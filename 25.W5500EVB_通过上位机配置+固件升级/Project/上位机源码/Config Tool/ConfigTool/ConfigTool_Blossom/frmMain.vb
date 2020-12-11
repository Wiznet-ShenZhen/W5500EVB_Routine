Imports system
Imports System.Text
Imports System.Net
Imports System.Net.Sockets
Imports System.Runtime.InteropServices


Public Class frmMain

#Region "private parameters"
    'Treeview root node
    Private rNode As Windows.Forms.TreeNode
    'globle SEC definition
    Private gSec As sSEC = New sSEC

    'udp broadcast port
    Private Const uPort As UInt16 = 1460
    'socket 
    Private WithEvents sckUDP As New clsSckUDP 'Searching & setting
    Private WithEvents sckTCP As New clsSckTCP 'Firmware updating
    'ip end point
    Private UdpRemoteEP As IPEndPoint
    'firmware uploading related parameters
    Private fLen As Long = 0
    Private fStrm As IO.FileStream
    Private Const bufLen As Integer = 1024
    Private buf(bufLen - 1) As Byte
    Private sOffset As Long = 0
    Private rxLen As Long = 0
    'searching message
    Private SrchMsg() As Byte = {Asc("F"), Asc("I"), Asc("N"), Asc("D")}
    Private FirmMsg() As Byte = {Asc("F"), Asc("I"), Asc("R"), Asc("M")}
    Private ClrEep() As Byte = {Asc("C"), Asc("L"), Asc("R"), Asc("E")}

#End Region
#Region "defined data item for combobox"
    'my data item for combobox
    Private Structure myDataItem
        Dim Name As String
        Dim rVal As Byte
        Public Sub New(ByVal _name As String, ByVal _value As Byte)
            Name = _name
            rVal = _value
        End Sub
        Public Overrides Function ToString() As String
            Return Me.Name
        End Function
    End Structure
#End Region
#Region "parsing functions"
    Private Function isIpAddr(ByVal ip As String) As Boolean
        If ip.Length > 0 Then
            Dim s() As String = Split(ip, ".")
            If s.Length = 4 Then
                For i As Int16 = 0 To 3
                    Try
                        If CType(s(i), Int16) > 255 Or CType(s(i), Int16) < 0 Then
                            Return False
                            Exit For
                        End If
                    Catch ex As Exception
                        Return False
                        Exit For
                    End Try
                Next
                Return True
            Else
                Return False
            End If
        Else
            Return False
        End If

    End Function
    'Convert bytes to MAC address string
    Private Function BytesToMacStr(ByVal b() As Byte) As String
        Dim sMac As String = ""
        For i As UInt16 = 0 To 5
            sMac += b(i).ToString("X2") & ":"
        Next
        sMac = sMac.Substring(0, sMac.Length - 1)
        Return sMac
    End Function
    'Converty MAC address string to bytes
    Private Function MacStrToBytes(ByVal Mac As String) As Byte()
        Dim bMac(5) As Byte
        Dim sMac() As String = Mac.Split(":")
        If sMac.Length = 6 Then
            For i As UInt16 = 0 To 5
                Try
                    bMac(i) = CType(sMac(i), Byte)
                Catch ex As Exception
                    MessageBox.Show("Invalid MAC address format!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation)
                End Try
            Next
        Else
            MessageBox.Show("Invalid MAC address format!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation)
        End If
        Return bMac
    End Function
    'Convert bytes to version string
    Private Function BytesToVerStr(ByVal b() As Byte) As String
        Return b(0).ToString & "." & b(1).ToString
    End Function
    'Convert version string to bytes

    'Calculate IP address string from bytes
    Private Function BytesToIpStr(ByVal b() As Byte) As String
        Dim ret As String = ""
        For i As UInt16 = 0 To 3
            ret += b(i).ToString & "."
        Next
        ret = ret.Substring(0, ret.Length - 1)
        Return ret
    End Function
    'Convert IP address string to bytes
    Private Function IpStrToBytes(ByVal Ip As String) As Byte()
        Dim ret(3) As Byte
        Dim sIP() As String = Ip.Split(".")
        If sIP.Length = 4 Then
            For i As UInt16 = 0 To 3
                Try
                    ret(i) = CType(sIP(i), Byte)
                Catch ex As Exception
                    MessageBox.Show("Invalid IP address format!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation)
                End Try
            Next
        Else
            MessageBox.Show("Invalid IP address format!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation)
        End If
        Return ret
    End Function
    'SEC Structure
    <StructLayout(LayoutKind.Sequential)> _
    Private Structure sSEC
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Op() As Byte
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim MAC() As Byte

        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Ver() As Byte
      
        
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim LocalIP() As Byte
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim GatewayIP() As Byte
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim SubnetMask() As Byte
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim DnsIP() As Byte

        Dim IpMode As Byte 'dhcp or static ip
        Dim Debug As Byte

      

        
    End Structure


    'Calculate SEC from bytes
    Private Function BytesToSec(ByVal dat() As Byte) As sSEC
        Dim sec As sSEC = New sSEC
        Dim size As Integer = Marshal.SizeOf(sec)
        Dim pPoint As IntPtr = Marshal.AllocHGlobal(size)
        Try
            Marshal.Copy(dat, 0, pPoint, size)
            sec = CType(Marshal.PtrToStructure(pPoint, sec.GetType()), sSEC)
            Return sec
        Finally
            Marshal.FreeHGlobal(pPoint)
        End Try
    End Function
    'Convert SEC to bytes
    Private Function SecToBytes(ByVal sec As sSEC) As Byte()
        Dim size As Integer = 0
        Try
            size = Marshal.SizeOf(sec)
        Catch ex As Exception
        End Try
        Dim buffer As IntPtr = Marshal.AllocHGlobal(size)
        'Try
        Marshal.StructureToPtr(sec, buffer, True)
        Dim bytes(size - 1) As Byte
        Marshal.Copy(buffer, bytes, 0, size)
        Return bytes
        'Catch ex As Exception
        'Return Nothing
        'Finally
        'Marshal.FreeHGlobal(buffer)
        'End Try
    End Function

#End Region
#Region "Delegate functions"

    Private Delegate Sub tvAddNodeCallback(ByVal tView As TreeView, ByVal pNode As Windows.Forms.TreeNode, ByVal sec As sSEC)
    Private Sub tvAddNode(ByVal tView As TreeView, ByVal pNode As Windows.Forms.TreeNode, ByVal sec As sSEC)

        If tView.InvokeRequired Then
            Dim d As New tvAddNodeCallback(AddressOf tvAddNode)
            Me.Invoke(d, New Object() {[tView], [pNode], [sec]})
        Else
            Dim nd As Windows.Forms.TreeNode
            Dim idx As Integer = 0
            Dim isExist As Boolean = False
            For Each nd In pNode.Nodes
                If nd.Text = BytesToMacStr(sec.MAC) Then
                    isExist = True
                    idx += 1
                    Exit For
                End If
            Next
            If Not isExist Then
                Dim cNode As Windows.Forms.TreeNode = pNode.Nodes.Add(BytesToMacStr(sec.MAC))
                cNode.Nodes.Add("Firmware Version: " & BytesToVerStr(sec.Ver))
                cNode.Tag = sec
                'set globle gSec
                gSec = sec
                'select the new added node
                tv.SelectedNode = cNode
            Else
                Dim cNode As Windows.Forms.TreeNode = pNode.Nodes(idx - 1)
                cNode.Tag = sec
                'set globle gSec
                gSec = sec
                cNode.Nodes(0).Text = "Firmware Version: " & BytesToVerStr(sec.Ver)
                'select the modified node
                tv.SelectedNode = cNode
            End If
            If pNode.Nodes.Count > 0 Then
                pNode.Expand()
            End If
        End If

    End Sub
    Private Delegate Sub enToolbarCallback(ByVal tsb As ToolStripButton, ByVal en As Boolean)
    Private Sub enToolBar(ByVal tsb As ToolStripButton, ByVal en As Boolean)
        If tsb.Owner.InvokeRequired Then
            Dim d As New enToolbarCallback(AddressOf enToolBar)
            Me.Invoke(d, New Object() {[tsb], [en]})
        Else
            tsb.Enabled = en
        End If
    End Sub
    Private Delegate Sub progressbarCallback(ByVal pb As ToolStripProgressBar, ByVal val As Integer)
    Private Sub progressbar(ByVal pb As ToolStripProgressBar, ByVal val As Integer)
        If pb.Owner.InvokeRequired Then
            Dim d As New progressbarCallback(AddressOf progressbar)
            Me.Invoke(d, New Object() {[pb], [val]})
        Else
            pb.Value = val
        End If
    End Sub
    Private Delegate Sub toolstripLblMsgCallback(ByVal tLabel As ToolStripLabel, ByVal msg As String)
    Private Sub tLabelmsg(ByVal tLabel As ToolStripLabel, ByVal msg As String)
        If tLabel.Owner.InvokeRequired Then
            Dim d As New toolstripLblMsgCallback(AddressOf tLabelmsg)
            Me.Invoke(d, New Object() {[tLabel], [msg]})
        Else
            tLabel.Text = msg
        End If
    End Sub
#End Region
#Region "TreeView Operation"
    Private Function tvAddRootNode(ByVal tView As TreeView, ByVal sName As String) As TreeNode
        Dim nd As TreeNode

        If sName.Length > 0 Then
            nd = tView.Nodes.Add(sName)
        Else
            nd = tView.Nodes.Add("Invalid node name!")
        End If
        Return nd

    End Function
    Private Function RemoveOneNode(ByVal tView As TreeView, ByVal pNode As Windows.Forms.TreeNode, ByVal nd As Windows.Forms.TreeNode) As String
        Dim ret As String = ""
        pNode.Nodes.Remove(nd)
        ret = nd.Text & " removed from the search results."
        Return ret
    End Function
    Private Sub RemoveAllNodes(ByVal pNode As Windows.Forms.TreeNode)
        pNode.Nodes.Clear()
    End Sub

#End Region
#Region "Main form control methods"
    Private Sub frmMain_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing

        Dim ret As DialogResult = MessageBox.Show("Are you sure to exit?", "Exit", MessageBoxButtons.YesNo, MessageBoxIcon.Information)
        If ret = Windows.Forms.DialogResult.Yes Then
            'close program
            End
        Else
            e.Cancel = True
        End If
    End Sub
    Private Function md5(ByVal s As String) As String
        Dim ret As String = ""
        Dim dataToHash As Byte() = (New ASCIIEncoding).GetBytes(s)
        Dim hashValue As Byte() = CType(System.Security.Cryptography.CryptoConfig.CreateFromName("MD5"), System.Security.Cryptography.HashAlgorithm).ComputeHash(dataToHash)
        Dim i As Integer
        For i = 0 To 15
            ret += hashValue(i).ToString("X2")

        Next
        Return ret

    End Function
    Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load



        'endof json test
        Me.Text = "IAP (ver" & Application.ProductVersion & ")"
        Dim b As Int16 = -9

        Dim h As Integer = &HFFF7

        'md5 test

        'Debug.Print(md5("abc"))


        'initialize controls
      
        'tree view
        rNode = tvAddRootNode(tv, "Network Devices")
        tv.SelectedNode = rNode

        'create ip end point object
        UdpRemoteEP = New IPEndPoint(IPAddress.Broadcast, uPort)
        sckUDP.Bind(0)

     

    End Sub

    'searching all modules in the same subnet using UDP broadcast
    Private Sub tsbSrchAll_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbSrchAll.Click
        Call RemoveAllNodes(rNode)
        sckUDP.SendTo(SrchMsg, UdpRemoteEP)
    End Sub
    'setting selected module
    Private Sub tsbSet_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbSet.Click
        Dim sec As sSEC = New sSEC
        'Setting indicator
        sec.Op = Encoding.ASCII.GetBytes("SETT")
        sec.Ver = gSec.Ver
        sec.MAC = gSec.MAC
        sec.LocalIP = IpStrToBytes(txtLIP.Text.Trim)
        sec.GatewayIP = IpStrToBytes(txtGw.Text.Trim)
        sec.SubnetMask = IpStrToBytes(txtSubnet.Text.Trim)
        sec.DnsIP = IpStrToBytes(txtDNS.Text.Trim)

        'debug
        sec.Debug = 1
 
        Dim dat() As Byte = SecToBytes(sec)
        'send data
        sckUDP.SendTo(dat, New IPEndPoint(IPAddress.Broadcast, uPort))

    End Sub
    Private Sub tv_AfterSelect(ByVal sender As System.Object, ByVal e As System.Windows.Forms.TreeViewEventArgs) Handles tv.AfterSelect
        Dim lv As UInt16 = tv.SelectedNode.Level
        Dim nd As Windows.Forms.TreeNode = New Windows.Forms.TreeNode
        If lv > 0 Then
            If lv = 1 Then
                nd = tv.SelectedNode
            ElseIf lv = 2 Then
                nd = tv.SelectedNode.Parent
            End If

            Dim sec As sSEC = CType(nd.Tag, sSEC)
          
            'Display values
            txtMac.Text = BytesToMacStr(sec.MAC)
            txtVer.Text = BytesToVerStr(sec.Ver)
            txtLIP.Text = BytesToIpStr(sec.LocalIP)
            txtSubnet.Text = BytesToIpStr(sec.SubnetMask)
            txtGw.Text = BytesToIpStr(sec.GatewayIP)
            txtDNS.Text = BytesToIpStr(sec.DnsIP)


            'enable controls
            tsbSet.Enabled = True
            tsbUpload.Enabled = True

        Else
            'diable controls
            tsbSet.Enabled = False
            tsbUpload.Enabled = False

        End If
    
    End Sub
    'Send firmware uploading command
    Private Sub tsbUpload_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbUpload.Click
        'open file dialog
        Dim sFile As String = ""
        ofd.FileName = ""
        ofd.ShowDialog()
        sFile = ofd.FileName
        If sFile.Length > 0 Then
            fStrm = New IO.FileStream(sFile, IO.FileMode.Open, IO.FileAccess.Read, IO.FileShare.ReadWrite)
            fLen = fStrm.Length
            'initial progress bar
            tpb.Maximum = fLen
            tpb.Value = 0
            'toolstrip label message
            tlblMsg.Text = "Firmware uploading..."
            rxLen = 0
            sOffset = 0
            If fLen < (120) * 1024 Then 'firmware length must < 60KByte
                'set toolstripprogressbar
                tpb.Value = 0
                tpb.Maximum = fLen
                'send firmware upload command
                Dim dat(9) As Byte
                dat(0) = Asc("F")
                dat(1) = Asc("I")
                dat(2) = Asc("R")
                dat(3) = Asc("M")

                For i As Int16 = 0 To 5
                    dat(i + 4) = gSec.MAC(i)
                Next
                'dat(7) = Math.Truncate(fLen / 256)
                'dat(8) = fLen Mod 256
                sckUDP.SendTo(dat, UdpRemoteEP)
            Else
                MessageBox.Show("The length of firmware file should be less than 60K bytes.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation)
                Exit Sub
            End If
        End If
    End Sub
    Private Sub tsbExit_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbExit.Click
        Me.Close()
    End Sub
#End Region
#Region "TCP/UDP processing"
    'UDP data arrival
    Private Sub sckUDP_DataArrival(ByVal len As Integer, ByVal dat() As Byte) Handles sckUDP.DataArrival
        Select Case Encoding.ASCII.GetString(dat, 0, 4)
            Case "FIND"
                'searching or setting response message
                Dim sec As sSEC = BytesToSec(dat)
                Call tvAddNode(tv, rNode, sec)
            Case "SETT"

                Dim sec As sSEC = BytesToSec(dat)
                Call tvAddNode(tv, rNode, sec)
                'enable tsmain

                enToolBar(tsbSet, True)
            Case "FIRM"
                '
                'firmware uploading command response message
                Dim sec As sSEC = BytesToSec(dat)
                Debug.Print("firware upload")
                Dim ipStr As String = sec.LocalIP(0).ToString & "." & sec.LocalIP(1).ToString & "." & sec.LocalIP(2).ToString & "." & sec.LocalIP(3).ToString
                Debug.Print("ip: " & ipStr)

                Dim tcpRemoteEP As IPEndPoint = New IPEndPoint(IPAddress.Parse(txtLIP.Text), 30000)
                sckTCP.Connect(tcpRemoteEP)
                'toolstrip label message
                tLabelmsg(tlblMsg, "Connecting with " & txtLIP.Text)

        End Select

    End Sub

    Private Sub sckUDP_SocketError(ByVal err As System.Exception) Handles sckUDP.SocketError
        MessageBox.Show(err.Message, "UDP Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
    End Sub

    Private Sub sckTCP_Connected() Handles sckTCP.Connected
        'send file length
        Dim bFLen(3) As Byte
        bFLen(3) = (fLen And &HFF000000) >> 24
        bFLen(2) = (fLen And &HFF0000) >> 16
        bFLen(1) = (fLen And &HFF00) >> 8
        bFLen(0) = fLen Mod 256

        sckTCP.Send(4, bFLen)
        'toolstrip label message
        tLabelmsg(tlblMsg, "Connected")
        Debug.Print("flen sent")
    End Sub

    Private Sub sckTCP_DataArrival(ByVal len As Integer, ByVal dat() As Byte) Handles sckTCP.DataArrival
        Dim n As Long = 0
        If len = 4 Then
            n = dat(3) * 2 ^ 24 + dat(2) * 2 ^ 16 + dat(1) * 2 ^ 8 + dat(0)
            Debug.Print(n)
            If n = fLen Then
                n = 0
            End If

        End If
        'send firmware data

        rxLen += n
        'update progress bar & label message
        progressbar(tpb, rxLen)
        tLabelmsg(tlblMsg, (Math.Truncate((rxLen / fLen) * 100)).ToString & "%")

        If rxLen < fLen Then
            If fLen - rxLen > bufLen Then
                fStrm.Seek(sOffset, IO.SeekOrigin.Begin)
                fStrm.Read(buf, 0, bufLen)
                sOffset += bufLen
                sckTCP.Send(bufLen, buf)
                Threading.Thread.Sleep(200)
            Else
                Dim b(fLen - rxLen - 1) As Byte
                fStrm.Seek(sOffset, IO.SeekOrigin.Begin)
                fStrm.Read(b, 0, fLen - rxLen)
                sOffset += fLen - rxLen
                sckTCP.Send(fLen - rxLen, b)
            End If
        ElseIf rxLen = fLen Then
            fStrm.Close()


            'MessageBox.Show("Firmware download over.")

        End If
    End Sub

    Private Sub sckTCP_Disconnected() Handles sckTCP.Disconnected
        Debug.Print("disconnected")

        MessageBox.Show("Firmware download over.", "Nuri ConfigTool", MessageBoxButtons.OK, MessageBoxIcon.Information)
    End Sub

    Private Sub sckTCP_SocketError(ByVal err As System.Exception) Handles sckTCP.SocketError
        MessageBox.Show(err.Message, "TCP Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
    End Sub
#End Region

    Private Sub btnClearEep_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim dat(9) As Byte
        dat(0) = Asc("C")
        dat(1) = Asc("L")
        dat(2) = Asc("R")
        dat(3) = Asc("E")

        For i As Int16 = 0 To 5
            dat(i + 4) = gSec.MAC(i)
        Next

        sckUDP.SendTo(dat, New IPEndPoint(IPAddress.Broadcast, uPort))
    End Sub

    Private Sub tsbFactory_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbReset.Click
        Dim dat(9) As Byte
        dat(0) = Asc("F")
        dat(1) = Asc("A")
        dat(2) = Asc("C")
        dat(3) = Asc("T")

        For i As Int16 = 0 To 5
            dat(i + 4) = gSec.MAC(i)
        Next

        sckUDP.SendTo(dat, New IPEndPoint(IPAddress.Broadcast, uPort))
    End Sub

    Private Sub Timer1_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Timer1.Tick
        'tsbSet_Click(sender, e)


    End Sub


End Class
