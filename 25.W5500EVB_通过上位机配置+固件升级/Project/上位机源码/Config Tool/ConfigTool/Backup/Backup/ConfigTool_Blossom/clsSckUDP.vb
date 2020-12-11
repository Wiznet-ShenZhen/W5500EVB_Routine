Imports System
Imports System.Net
Imports System.Net.Sockets
Public Class clsSckUDP
    Public Event DataArrival(ByVal len As Integer, ByVal dat() As Byte)
    Public Event SocketError(ByVal err As Exception)

    Private Class StateObject
        'work socket
        Friend wSck As Socket = Nothing
        'remote end point
        Friend RemoteEP As IPEndPoint
        'size of receive buffer
        Friend Const bufSize As Integer = 2048
        'receive buffer
        Friend buf(bufSize - 1) As Byte
    End Class

    Private sck As Socket
    Private _isBroadcast As Boolean
    Public Property isBroadcast() As Boolean
        Get
            Return _isBroadcast
        End Get
        Set(ByVal value As Boolean)
            _isBroadcast = value
        End Set
    End Property
    Private Function GetRndPort() As UInt16
        Dim ret As UInt16 = 0
        Dim rnd As System.Random = New Random(Now.Millisecond)
        ret = rnd.Next(1, 65535)
        Return ret
    End Function
    Public Sub Bind(ByVal port As UInt16)
        Try
            'If port = 0 Then port = GetRndPort()
            Dim lIp As IPAddress = Nothing
            For Each ip As IPAddress In Dns.GetHostEntry(Dns.GetHostName).AddressList
                If ip.AddressFamily = AddressFamily.InterNetwork Then
                    lIp = ip
                    Exit For
                End If
            Next

            Dim LocalEP As EndPoint = New IPEndPoint(lIp, port)
            sck = New Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp)
            sck.EnableBroadcast = True

            sck.Bind(LocalEP)
            Dim remoteEP As IPEndPoint = New IPEndPoint(IPAddress.Any, port) 'broadcast
            ReceiveFrom(remoteEP)
        Catch ex As Exception
            RaiseEvent SocketError(ex)
        End Try
    End Sub

    Private Sub ReceiveFrom(ByVal remoteEP As IPEndPoint)
        Try
            'create the state object
            Dim state As New StateObject
            state.wSck = sck
            state.RemoteEP = remoteEP
            sck.BeginReceiveFrom(state.buf, 0, StateObject.bufSize, SocketFlags.None, remoteEP, New AsyncCallback(AddressOf ReceiveFromCallback), state)
        Catch ex As Exception
            RaiseEvent SocketError(ex)
        End Try
    End Sub
    Private Sub ReceiveFromCallback(ByVal ar As IAsyncResult)
        If ar.IsCompleted Then
            Dim state As StateObject = CType(ar.AsyncState, StateObject)
            sck = state.wSck

            Dim bytesRead As Integer = 0

            Try
                bytesRead = sck.EndReceiveFrom(ar, state.remoteEP)
                If bytesRead > 0 Then
                    'raise DataArrival event
                    Dim dat(bytesRead - 1) As Byte
                    Array.ConstrainedCopy(state.buf, 0, dat, 0, bytesRead)
                    RaiseEvent DataArrival(bytesRead, dat)
                End If
                'receive again
                sck.BeginReceiveFrom(state.buf, 0, StateObject.bufSize, SocketFlags.None, state.remoteEP, New AsyncCallback(AddressOf ReceiveFromCallback), state)
            Catch ex As Exception
                'raise error event
                RaiseEvent SocketError(ex)
            End Try
        End If

    End Sub
    Public Sub SendTo(ByVal dat() As Byte, ByVal remoteEP As IPEndPoint)
        Try
            'begin sending the data
            sck.BeginSendTo(dat, 0, dat.Length, SocketFlags.None, remoteEP, New AsyncCallback(AddressOf SendToCallback), sck)
        Catch ex As Exception
            RaiseEvent SocketError(ex)
        End Try

    End Sub
    Private Sub SendToCallback(ByVal ar As IAsyncResult)
        If ar.IsCompleted Then
            Dim sck As Socket = CType(ar.AsyncState, Socket)
            Try
                Dim bytesSent As Integer = 0
                bytesSent = sck.EndSendTo(ar)
            Catch ex As Exception
                RaiseEvent SocketError(ex)
            End Try
        End If

    End Sub
    Public Sub Close()
        sck.Close()

    End Sub
End Class
