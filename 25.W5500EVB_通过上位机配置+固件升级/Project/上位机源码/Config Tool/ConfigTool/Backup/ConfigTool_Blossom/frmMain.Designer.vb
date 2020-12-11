<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmMain
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmMain))
        Me.tsMain = New System.Windows.Forms.ToolStrip
        Me.tsbSrchAll = New System.Windows.Forms.ToolStripButton
        Me.tsbSet = New System.Windows.Forms.ToolStripButton
        Me.tsbUpload = New System.Windows.Forms.ToolStripButton
        Me.tsbFactory = New System.Windows.Forms.ToolStripButton
        Me.ToolStripSeparator4 = New System.Windows.Forms.ToolStripSeparator
        Me.tsbExit = New System.Windows.Forms.ToolStripButton
        Me.ofd = New System.Windows.Forms.OpenFileDialog
        Me.ssMain = New System.Windows.Forms.StatusStrip
        Me.tLblState = New System.Windows.Forms.ToolStripStatusLabel
        Me.tpb = New System.Windows.Forms.ToolStripProgressBar
        Me.tlblMsg = New System.Windows.Forms.ToolStripStatusLabel
        Me.tv = New System.Windows.Forms.TreeView
        Me.Tab = New System.Windows.Forms.TabControl
        Me.TabPage1 = New System.Windows.Forms.TabPage
        Me.GroupBox5 = New System.Windows.Forms.GroupBox
        Me.cboFlow = New System.Windows.Forms.ComboBox
        Me.cboStopBit = New System.Windows.Forms.ComboBox
        Me.cboParity = New System.Windows.Forms.ComboBox
        Me.cboDataBit = New System.Windows.Forms.ComboBox
        Me.cboBaud = New System.Windows.Forms.ComboBox
        Me.Label18 = New System.Windows.Forms.Label
        Me.Label17 = New System.Windows.Forms.Label
        Me.Label16 = New System.Windows.Forms.Label
        Me.Label15 = New System.Windows.Forms.Label
        Me.Label14 = New System.Windows.Forms.Label
        Me.GroupBox1 = New System.Windows.Forms.GroupBox
        Me.Label28 = New System.Windows.Forms.Label
        Me.txtDNS = New System.Windows.Forms.TextBox
        Me.txtGw = New System.Windows.Forms.TextBox
        Me.txtSubnet = New System.Windows.Forms.TextBox
        Me.txtLIP = New System.Windows.Forms.TextBox
        Me.Label4 = New System.Windows.Forms.Label
        Me.Label3 = New System.Windows.Forms.Label
        Me.Label2 = New System.Windows.Forms.Label
        Me.Label1 = New System.Windows.Forms.Label
        Me.rbStatic = New System.Windows.Forms.RadioButton
        Me.rbDHCP = New System.Windows.Forms.RadioButton
        Me.TabPage2 = New System.Windows.Forms.TabPage
        Me.GroupBox3 = New System.Windows.Forms.GroupBox
        Me.txtRhost = New System.Windows.Forms.TextBox
        Me.Label7 = New System.Windows.Forms.Label
        Me.txtRport = New System.Windows.Forms.TextBox
        Me.Label6 = New System.Windows.Forms.Label
        Me.txtLport = New System.Windows.Forms.TextBox
        Me.Label5 = New System.Windows.Forms.Label
        Me.GroupBox2 = New System.Windows.Forms.GroupBox
        Me.rbUdp = New System.Windows.Forms.RadioButton
        Me.rbTcpMixed = New System.Windows.Forms.RadioButton
        Me.rbTcpServer = New System.Windows.Forms.RadioButton
        Me.rbTcpClient = New System.Windows.Forms.RadioButton
        Me.TabPage3 = New System.Windows.Forms.TabPage
        Me.txtPwd = New System.Windows.Forms.TextBox
        Me.Label21 = New System.Windows.Forms.Label
        Me.txtSerialNumber = New System.Windows.Forms.TextBox
        Me.txtDeviceName = New System.Windows.Forms.TextBox
        Me.Label25 = New System.Windows.Forms.Label
        Me.Label22 = New System.Windows.Forms.Label
        Me.GroupBox9 = New System.Windows.Forms.GroupBox
        Me.txtReconn = New System.Windows.Forms.TextBox
        Me.txtInactivity = New System.Windows.Forms.TextBox
        Me.lblReconn = New System.Windows.Forms.Label
        Me.lblInactivity = New System.Windows.Forms.Label
        Me.txtNagle = New System.Windows.Forms.TextBox
        Me.Label24 = New System.Windows.Forms.Label
        Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
        Me.tsMain.SuspendLayout()
        Me.ssMain.SuspendLayout()
        Me.Tab.SuspendLayout()
        Me.TabPage1.SuspendLayout()
        Me.GroupBox5.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        Me.TabPage2.SuspendLayout()
        Me.GroupBox3.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.TabPage3.SuspendLayout()
        Me.GroupBox9.SuspendLayout()
        Me.SuspendLayout()
        '
        'tsMain
        '
        Me.tsMain.ImageScalingSize = New System.Drawing.Size(32, 32)
        Me.tsMain.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.tsbSrchAll, Me.tsbSet, Me.tsbUpload, Me.tsbFactory, Me.ToolStripSeparator4, Me.tsbExit})
        Me.tsMain.Location = New System.Drawing.Point(0, 0)
        Me.tsMain.Name = "tsMain"
        Me.tsMain.RenderMode = System.Windows.Forms.ToolStripRenderMode.System
        Me.tsMain.Size = New System.Drawing.Size(557, 39)
        Me.tsMain.TabIndex = 1
        Me.tsMain.Text = "ToolStrip1"
        '
        'tsbSrchAll
        '
        Me.tsbSrchAll.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.tsbSrchAll.Image = CType(resources.GetObject("tsbSrchAll.Image"), System.Drawing.Image)
        Me.tsbSrchAll.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None
        Me.tsbSrchAll.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbSrchAll.Margin = New System.Windows.Forms.Padding(1)
        Me.tsbSrchAll.Name = "tsbSrchAll"
        Me.tsbSrchAll.Size = New System.Drawing.Size(82, 37)
        Me.tsbSrchAll.Text = "Search"
        Me.tsbSrchAll.ToolTipText = "Search"
        '
        'tsbSet
        '
        Me.tsbSet.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.tsbSet.Image = CType(resources.GetObject("tsbSet.Image"), System.Drawing.Image)
        Me.tsbSet.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None
        Me.tsbSet.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbSet.Name = "tsbSet"
        Me.tsbSet.Size = New System.Drawing.Size(81, 36)
        Me.tsbSet.Text = "Setting"
        '
        'tsbUpload
        '
        Me.tsbUpload.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.tsbUpload.Image = CType(resources.GetObject("tsbUpload.Image"), System.Drawing.Image)
        Me.tsbUpload.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None
        Me.tsbUpload.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbUpload.Name = "tsbUpload"
        Me.tsbUpload.Size = New System.Drawing.Size(138, 36)
        Me.tsbUpload.Text = "Firmware Upload"
        '
        'tsbFactory
        '
        Me.tsbFactory.Image = CType(resources.GetObject("tsbFactory.Image"), System.Drawing.Image)
        Me.tsbFactory.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbFactory.Name = "tsbFactory"
        Me.tsbFactory.Size = New System.Drawing.Size(119, 36)
        Me.tsbFactory.Text = "Factory Reset"
        '
        'ToolStripSeparator4
        '
        Me.ToolStripSeparator4.Name = "ToolStripSeparator4"
        Me.ToolStripSeparator4.Size = New System.Drawing.Size(6, 39)
        '
        'tsbExit
        '
        Me.tsbExit.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.tsbExit.Image = CType(resources.GetObject("tsbExit.Image"), System.Drawing.Image)
        Me.tsbExit.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None
        Me.tsbExit.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbExit.Name = "tsbExit"
        Me.tsbExit.Size = New System.Drawing.Size(62, 36)
        Me.tsbExit.Text = "Exit"
        '
        'ofd
        '
        Me.ofd.Filter = "Bin files|*.bin|All files|*.*"
        '
        'ssMain
        '
        Me.ssMain.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.tLblState, Me.tpb, Me.tlblMsg})
        Me.ssMain.Location = New System.Drawing.Point(0, 426)
        Me.ssMain.Name = "ssMain"
        Me.ssMain.Size = New System.Drawing.Size(557, 22)
        Me.ssMain.TabIndex = 14
        '
        'tLblState
        '
        Me.tLblState.Image = CType(resources.GetObject("tLblState.Image"), System.Drawing.Image)
        Me.tLblState.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None
        Me.tLblState.Name = "tLblState"
        Me.tLblState.Size = New System.Drawing.Size(82, 17)
        Me.tLblState.Text = "ConfigTool"
        '
        'tpb
        '
        Me.tpb.Name = "tpb"
        Me.tpb.Size = New System.Drawing.Size(150, 16)
        '
        'tlblMsg
        '
        Me.tlblMsg.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text
        Me.tlblMsg.Image = CType(resources.GetObject("tlblMsg.Image"), System.Drawing.Image)
        Me.tlblMsg.Margin = New System.Windows.Forms.Padding(5, 3, 0, 2)
        Me.tlblMsg.Name = "tlblMsg"
        Me.tlblMsg.Size = New System.Drawing.Size(0, 17)
        '
        'tv
        '
        Me.tv.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.tv.HideSelection = False
        Me.tv.Location = New System.Drawing.Point(5, 45)
        Me.tv.Name = "tv"
        Me.tv.ShowNodeToolTips = True
        Me.tv.Size = New System.Drawing.Size(232, 375)
        Me.tv.TabIndex = 15
        '
        'Tab
        '
        Me.Tab.Controls.Add(Me.TabPage1)
        Me.Tab.Controls.Add(Me.TabPage2)
        Me.Tab.Controls.Add(Me.TabPage3)
        Me.Tab.Location = New System.Drawing.Point(243, 42)
        Me.Tab.Name = "Tab"
        Me.Tab.SelectedIndex = 0
        Me.Tab.Size = New System.Drawing.Size(313, 380)
        Me.Tab.TabIndex = 16
        '
        'TabPage1
        '
        Me.TabPage1.Controls.Add(Me.GroupBox5)
        Me.TabPage1.Controls.Add(Me.GroupBox1)
        Me.TabPage1.Location = New System.Drawing.Point(4, 21)
        Me.TabPage1.Name = "TabPage1"
        Me.TabPage1.Padding = New System.Windows.Forms.Padding(3)
        Me.TabPage1.Size = New System.Drawing.Size(305, 355)
        Me.TabPage1.TabIndex = 0
        Me.TabPage1.Text = "Device"
        Me.TabPage1.UseVisualStyleBackColor = True
        '
        'GroupBox5
        '
        Me.GroupBox5.Controls.Add(Me.cboFlow)
        Me.GroupBox5.Controls.Add(Me.cboStopBit)
        Me.GroupBox5.Controls.Add(Me.cboParity)
        Me.GroupBox5.Controls.Add(Me.cboDataBit)
        Me.GroupBox5.Controls.Add(Me.cboBaud)
        Me.GroupBox5.Controls.Add(Me.Label18)
        Me.GroupBox5.Controls.Add(Me.Label17)
        Me.GroupBox5.Controls.Add(Me.Label16)
        Me.GroupBox5.Controls.Add(Me.Label15)
        Me.GroupBox5.Controls.Add(Me.Label14)
        Me.GroupBox5.Location = New System.Drawing.Point(4, 183)
        Me.GroupBox5.Name = "GroupBox5"
        Me.GroupBox5.Size = New System.Drawing.Size(295, 166)
        Me.GroupBox5.TabIndex = 15
        Me.GroupBox5.TabStop = False
        Me.GroupBox5.Text = "COM port settings"
        '
        'cboFlow
        '
        Me.cboFlow.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cboFlow.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.cboFlow.FormattingEnabled = True
        Me.cboFlow.Location = New System.Drawing.Point(138, 137)
        Me.cboFlow.Margin = New System.Windows.Forms.Padding(3, 6, 3, 6)
        Me.cboFlow.Name = "cboFlow"
        Me.cboFlow.Size = New System.Drawing.Size(131, 23)
        Me.cboFlow.TabIndex = 19
        '
        'cboStopBit
        '
        Me.cboStopBit.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cboStopBit.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.cboStopBit.FormattingEnabled = True
        Me.cboStopBit.Location = New System.Drawing.Point(138, 109)
        Me.cboStopBit.Margin = New System.Windows.Forms.Padding(3, 6, 3, 6)
        Me.cboStopBit.Name = "cboStopBit"
        Me.cboStopBit.Size = New System.Drawing.Size(131, 23)
        Me.cboStopBit.TabIndex = 18
        '
        'cboParity
        '
        Me.cboParity.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cboParity.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.cboParity.FormattingEnabled = True
        Me.cboParity.Location = New System.Drawing.Point(138, 79)
        Me.cboParity.Margin = New System.Windows.Forms.Padding(3, 6, 3, 6)
        Me.cboParity.Name = "cboParity"
        Me.cboParity.Size = New System.Drawing.Size(131, 23)
        Me.cboParity.TabIndex = 17
        '
        'cboDataBit
        '
        Me.cboDataBit.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cboDataBit.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.cboDataBit.FormattingEnabled = True
        Me.cboDataBit.Location = New System.Drawing.Point(138, 49)
        Me.cboDataBit.Margin = New System.Windows.Forms.Padding(3, 6, 3, 6)
        Me.cboDataBit.Name = "cboDataBit"
        Me.cboDataBit.Size = New System.Drawing.Size(131, 23)
        Me.cboDataBit.TabIndex = 16
        '
        'cboBaud
        '
        Me.cboBaud.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cboBaud.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.cboBaud.FormattingEnabled = True
        Me.cboBaud.Location = New System.Drawing.Point(138, 19)
        Me.cboBaud.Margin = New System.Windows.Forms.Padding(3, 6, 3, 6)
        Me.cboBaud.Name = "cboBaud"
        Me.cboBaud.Size = New System.Drawing.Size(131, 23)
        Me.cboBaud.TabIndex = 15
        '
        'Label18
        '
        Me.Label18.AutoSize = True
        Me.Label18.Location = New System.Drawing.Point(35, 142)
        Me.Label18.Name = "Label18"
        Me.Label18.Size = New System.Drawing.Size(97, 12)
        Me.Label18.TabIndex = 14
        Me.Label18.Text = "Flow Control(&F):"
        '
        'Label17
        '
        Me.Label17.AutoSize = True
        Me.Label17.Location = New System.Drawing.Point(63, 114)
        Me.Label17.Name = "Label17"
        Me.Label17.Size = New System.Drawing.Size(70, 12)
        Me.Label17.TabIndex = 13
        Me.Label17.Text = "Stop Bit(&S):"
        '
        'Label16
        '
        Me.Label16.AutoSize = True
        Me.Label16.Location = New System.Drawing.Point(73, 84)
        Me.Label16.Name = "Label16"
        Me.Label16.Size = New System.Drawing.Size(59, 12)
        Me.Label16.TabIndex = 12
        Me.Label16.Text = "Parity(&P):"
        '
        'Label15
        '
        Me.Label15.AutoSize = True
        Me.Label15.Location = New System.Drawing.Point(63, 54)
        Me.Label15.Name = "Label15"
        Me.Label15.Size = New System.Drawing.Size(70, 12)
        Me.Label15.TabIndex = 11
        Me.Label15.Text = "Data Bit(&D):"
        '
        'Label14
        '
        Me.Label14.AutoSize = True
        Me.Label14.Location = New System.Drawing.Point(50, 24)
        Me.Label14.Name = "Label14"
        Me.Label14.Size = New System.Drawing.Size(85, 12)
        Me.Label14.TabIndex = 10
        Me.Label14.Text = "Baud Rate(&R):"
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.Label28)
        Me.GroupBox1.Controls.Add(Me.txtDNS)
        Me.GroupBox1.Controls.Add(Me.txtGw)
        Me.GroupBox1.Controls.Add(Me.txtSubnet)
        Me.GroupBox1.Controls.Add(Me.txtLIP)
        Me.GroupBox1.Controls.Add(Me.Label4)
        Me.GroupBox1.Controls.Add(Me.Label3)
        Me.GroupBox1.Controls.Add(Me.Label2)
        Me.GroupBox1.Controls.Add(Me.Label1)
        Me.GroupBox1.Controls.Add(Me.rbStatic)
        Me.GroupBox1.Controls.Add(Me.rbDHCP)
        Me.GroupBox1.Location = New System.Drawing.Point(4, 7)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(295, 170)
        Me.GroupBox1.TabIndex = 14
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "Device network settings"
        '
        'Label28
        '
        Me.Label28.BackColor = System.Drawing.Color.Silver
        Me.Label28.Location = New System.Drawing.Point(12, 59)
        Me.Label28.Name = "Label28"
        Me.Label28.Size = New System.Drawing.Size(270, 2)
        Me.Label28.TabIndex = 29
        '
        'txtDNS
        '
        Me.txtDNS.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtDNS.Location = New System.Drawing.Point(138, 141)
        Me.txtDNS.Name = "txtDNS"
        Me.txtDNS.Size = New System.Drawing.Size(131, 21)
        Me.txtDNS.TabIndex = 21
        '
        'txtGw
        '
        Me.txtGw.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtGw.Location = New System.Drawing.Point(138, 116)
        Me.txtGw.Name = "txtGw"
        Me.txtGw.Size = New System.Drawing.Size(131, 21)
        Me.txtGw.TabIndex = 20
        '
        'txtSubnet
        '
        Me.txtSubnet.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtSubnet.Location = New System.Drawing.Point(138, 91)
        Me.txtSubnet.Name = "txtSubnet"
        Me.txtSubnet.Size = New System.Drawing.Size(131, 21)
        Me.txtSubnet.TabIndex = 19
        '
        'txtLIP
        '
        Me.txtLIP.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtLIP.Location = New System.Drawing.Point(138, 66)
        Me.txtLIP.Name = "txtLIP"
        Me.txtLIP.Size = New System.Drawing.Size(131, 21)
        Me.txtLIP.TabIndex = 18
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Location = New System.Drawing.Point(59, 144)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(73, 12)
        Me.Label4.TabIndex = 17
        Me.Label4.Text = "DNS server:"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(73, 96)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(59, 12)
        Me.Label3.TabIndex = 16
        Me.Label3.Text = "Gateway:"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(50, 121)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(83, 12)
        Me.Label2.TabIndex = 15
        Me.Label2.Text = "Subnet mask:"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(21, 69)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(112, 12)
        Me.Label1.TabIndex = 14
        Me.Label1.Text = "Device IP address:"
        '
        'rbStatic
        '
        Me.rbStatic.AutoSize = True
        Me.rbStatic.Location = New System.Drawing.Point(23, 17)
        Me.rbStatic.Name = "rbStatic"
        Me.rbStatic.Size = New System.Drawing.Size(179, 16)
        Me.rbStatic.TabIndex = 5
        Me.rbStatic.TabStop = True
        Me.rbStatic.Text = "Using the follow IP Address"
        Me.rbStatic.UseVisualStyleBackColor = True
        '
        'rbDHCP
        '
        Me.rbDHCP.AutoSize = True
        Me.rbDHCP.Location = New System.Drawing.Point(23, 39)
        Me.rbDHCP.Name = "rbDHCP"
        Me.rbDHCP.Size = New System.Drawing.Size(212, 16)
        Me.rbDHCP.TabIndex = 3
        Me.rbDHCP.TabStop = True
        Me.rbDHCP.Text = "Get IP address from DHCP server"
        Me.rbDHCP.UseVisualStyleBackColor = True
        '
        'TabPage2
        '
        Me.TabPage2.Controls.Add(Me.GroupBox3)
        Me.TabPage2.Controls.Add(Me.GroupBox2)
        Me.TabPage2.Location = New System.Drawing.Point(4, 21)
        Me.TabPage2.Name = "TabPage2"
        Me.TabPage2.Padding = New System.Windows.Forms.Padding(3)
        Me.TabPage2.Size = New System.Drawing.Size(305, 355)
        Me.TabPage2.TabIndex = 1
        Me.TabPage2.Text = "Connection"
        Me.TabPage2.UseVisualStyleBackColor = True
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.txtRhost)
        Me.GroupBox3.Controls.Add(Me.Label7)
        Me.GroupBox3.Controls.Add(Me.txtRport)
        Me.GroupBox3.Controls.Add(Me.Label6)
        Me.GroupBox3.Controls.Add(Me.txtLport)
        Me.GroupBox3.Controls.Add(Me.Label5)
        Me.GroupBox3.Location = New System.Drawing.Point(3, 80)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(296, 85)
        Me.GroupBox3.TabIndex = 1
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "Communication"
        '
        'txtRhost
        '
        Me.txtRhost.Location = New System.Drawing.Point(92, 49)
        Me.txtRhost.Name = "txtRhost"
        Me.txtRhost.Size = New System.Drawing.Size(197, 21)
        Me.txtRhost.TabIndex = 5
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Location = New System.Drawing.Point(6, 52)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(80, 12)
        Me.Label7.TabIndex = 4
        Me.Label7.Text = "Remote host:"
        '
        'txtRport
        '
        Me.txtRport.Location = New System.Drawing.Point(236, 22)
        Me.txtRport.Name = "txtRport"
        Me.txtRport.Size = New System.Drawing.Size(53, 21)
        Me.txtRport.TabIndex = 3
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Location = New System.Drawing.Point(153, 25)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(77, 12)
        Me.Label6.TabIndex = 2
        Me.Label6.Text = "Remote port:"
        '
        'txtLport
        '
        Me.txtLport.Location = New System.Drawing.Point(77, 22)
        Me.txtLport.Name = "txtLport"
        Me.txtLport.Size = New System.Drawing.Size(53, 21)
        Me.txtLport.TabIndex = 1
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(6, 25)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(65, 12)
        Me.Label5.TabIndex = 0
        Me.Label5.Text = "Local port:"
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.rbUdp)
        Me.GroupBox2.Controls.Add(Me.rbTcpMixed)
        Me.GroupBox2.Controls.Add(Me.rbTcpServer)
        Me.GroupBox2.Controls.Add(Me.rbTcpClient)
        Me.GroupBox2.Location = New System.Drawing.Point(3, 6)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(296, 68)
        Me.GroupBox2.TabIndex = 0
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "Working mode"
        '
        'rbUdp
        '
        Me.rbUdp.AutoSize = True
        Me.rbUdp.Location = New System.Drawing.Point(177, 42)
        Me.rbUdp.Name = "rbUdp"
        Me.rbUdp.Size = New System.Drawing.Size(47, 16)
        Me.rbUdp.TabIndex = 3
        Me.rbUdp.TabStop = True
        Me.rbUdp.Text = "UDP"
        Me.rbUdp.UseVisualStyleBackColor = True
        '
        'rbTcpMixed
        '
        Me.rbTcpMixed.AutoSize = True
        Me.rbTcpMixed.Location = New System.Drawing.Point(13, 42)
        Me.rbTcpMixed.Name = "rbTcpMixed"
        Me.rbTcpMixed.Size = New System.Drawing.Size(87, 16)
        Me.rbTcpMixed.TabIndex = 2
        Me.rbTcpMixed.TabStop = True
        Me.rbTcpMixed.Text = "TCP mixed"
        Me.rbTcpMixed.UseVisualStyleBackColor = True
        '
        'rbTcpServer
        '
        Me.rbTcpServer.AutoSize = True
        Me.rbTcpServer.Location = New System.Drawing.Point(177, 20)
        Me.rbTcpServer.Name = "rbTcpServer"
        Me.rbTcpServer.Size = New System.Drawing.Size(87, 16)
        Me.rbTcpServer.TabIndex = 1
        Me.rbTcpServer.TabStop = True
        Me.rbTcpServer.Text = "TCP server"
        Me.rbTcpServer.UseVisualStyleBackColor = True
        '
        'rbTcpClient
        '
        Me.rbTcpClient.AutoSize = True
        Me.rbTcpClient.Location = New System.Drawing.Point(13, 20)
        Me.rbTcpClient.Name = "rbTcpClient"
        Me.rbTcpClient.Size = New System.Drawing.Size(82, 16)
        Me.rbTcpClient.TabIndex = 0
        Me.rbTcpClient.TabStop = True
        Me.rbTcpClient.Text = "TCP client"
        Me.rbTcpClient.UseVisualStyleBackColor = True
        '
        'TabPage3
        '
        Me.TabPage3.Controls.Add(Me.txtPwd)
        Me.TabPage3.Controls.Add(Me.Label21)
        Me.TabPage3.Controls.Add(Me.txtSerialNumber)
        Me.TabPage3.Controls.Add(Me.txtDeviceName)
        Me.TabPage3.Controls.Add(Me.Label25)
        Me.TabPage3.Controls.Add(Me.Label22)
        Me.TabPage3.Controls.Add(Me.GroupBox9)
        Me.TabPage3.Location = New System.Drawing.Point(4, 21)
        Me.TabPage3.Name = "TabPage3"
        Me.TabPage3.Padding = New System.Windows.Forms.Padding(3)
        Me.TabPage3.Size = New System.Drawing.Size(305, 355)
        Me.TabPage3.TabIndex = 2
        Me.TabPage3.Text = "Option"
        Me.TabPage3.UseVisualStyleBackColor = True
        '
        'txtPwd
        '
        Me.txtPwd.Location = New System.Drawing.Point(115, 75)
        Me.txtPwd.Name = "txtPwd"
        Me.txtPwd.Size = New System.Drawing.Size(177, 21)
        Me.txtPwd.TabIndex = 8
        '
        'Label21
        '
        Me.Label21.AutoSize = True
        Me.Label21.Location = New System.Drawing.Point(12, 78)
        Me.Label21.Name = "Label21"
        Me.Label21.Size = New System.Drawing.Size(66, 12)
        Me.Label21.TabIndex = 7
        Me.Label21.Text = "Password:"
        '
        'txtSerialNumber
        '
        Me.txtSerialNumber.Location = New System.Drawing.Point(115, 49)
        Me.txtSerialNumber.Name = "txtSerialNumber"
        Me.txtSerialNumber.ReadOnly = True
        Me.txtSerialNumber.Size = New System.Drawing.Size(177, 21)
        Me.txtSerialNumber.TabIndex = 6
        '
        'txtDeviceName
        '
        Me.txtDeviceName.Location = New System.Drawing.Point(115, 22)
        Me.txtDeviceName.Name = "txtDeviceName"
        Me.txtDeviceName.Size = New System.Drawing.Size(177, 21)
        Me.txtDeviceName.TabIndex = 5
        '
        'Label25
        '
        Me.Label25.AutoSize = True
        Me.Label25.Location = New System.Drawing.Point(12, 52)
        Me.Label25.Name = "Label25"
        Me.Label25.Size = New System.Drawing.Size(88, 12)
        Me.Label25.TabIndex = 4
        Me.Label25.Text = "Serial number:"
        '
        'Label22
        '
        Me.Label22.AutoSize = True
        Me.Label22.Location = New System.Drawing.Point(12, 25)
        Me.Label22.Name = "Label22"
        Me.Label22.Size = New System.Drawing.Size(83, 12)
        Me.Label22.TabIndex = 3
        Me.Label22.Text = "Device name:"
        '
        'GroupBox9
        '
        Me.GroupBox9.Controls.Add(Me.txtReconn)
        Me.GroupBox9.Controls.Add(Me.txtInactivity)
        Me.GroupBox9.Controls.Add(Me.lblReconn)
        Me.GroupBox9.Controls.Add(Me.lblInactivity)
        Me.GroupBox9.Controls.Add(Me.txtNagle)
        Me.GroupBox9.Controls.Add(Me.Label24)
        Me.GroupBox9.Location = New System.Drawing.Point(6, 117)
        Me.GroupBox9.Name = "GroupBox9"
        Me.GroupBox9.Size = New System.Drawing.Size(293, 98)
        Me.GroupBox9.TabIndex = 2
        Me.GroupBox9.TabStop = False
        Me.GroupBox9.Text = "Timers"
        '
        'txtReconn
        '
        Me.txtReconn.Location = New System.Drawing.Point(109, 68)
        Me.txtReconn.Name = "txtReconn"
        Me.txtReconn.Size = New System.Drawing.Size(65, 21)
        Me.txtReconn.TabIndex = 9
        '
        'txtInactivity
        '
        Me.txtInactivity.Location = New System.Drawing.Point(109, 41)
        Me.txtInactivity.Name = "txtInactivity"
        Me.txtInactivity.Size = New System.Drawing.Size(65, 21)
        Me.txtInactivity.TabIndex = 8
        '
        'lblReconn
        '
        Me.lblReconn.AutoSize = True
        Me.lblReconn.Location = New System.Drawing.Point(6, 71)
        Me.lblReconn.Name = "lblReconn"
        Me.lblReconn.Size = New System.Drawing.Size(86, 12)
        Me.lblReconn.TabIndex = 7
        Me.lblReconn.Text = "Reconnection:"
        '
        'lblInactivity
        '
        Me.lblInactivity.AutoSize = True
        Me.lblInactivity.Location = New System.Drawing.Point(6, 44)
        Me.lblInactivity.Name = "lblInactivity"
        Me.lblInactivity.Size = New System.Drawing.Size(58, 12)
        Me.lblInactivity.TabIndex = 6
        Me.lblInactivity.Text = "Inactivity:"
        '
        'txtNagle
        '
        Me.txtNagle.Location = New System.Drawing.Point(109, 14)
        Me.txtNagle.Name = "txtNagle"
        Me.txtNagle.Size = New System.Drawing.Size(65, 21)
        Me.txtNagle.TabIndex = 5
        '
        'Label24
        '
        Me.Label24.AutoSize = True
        Me.Label24.Location = New System.Drawing.Point(6, 17)
        Me.Label24.Name = "Label24"
        Me.Label24.Size = New System.Drawing.Size(97, 12)
        Me.Label24.TabIndex = 0
        Me.Label24.Text = "Nagle wait time:"
        '
        'Timer1
        '
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(7.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(557, 448)
        Me.Controls.Add(Me.Tab)
        Me.Controls.Add(Me.tv)
        Me.Controls.Add(Me.ssMain)
        Me.Controls.Add(Me.tsMain)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.Name = "frmMain"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "frmMain"
        Me.tsMain.ResumeLayout(False)
        Me.tsMain.PerformLayout()
        Me.ssMain.ResumeLayout(False)
        Me.ssMain.PerformLayout()
        Me.Tab.ResumeLayout(False)
        Me.TabPage1.ResumeLayout(False)
        Me.GroupBox5.ResumeLayout(False)
        Me.GroupBox5.PerformLayout()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.TabPage2.ResumeLayout(False)
        Me.GroupBox3.ResumeLayout(False)
        Me.GroupBox3.PerformLayout()
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.TabPage3.ResumeLayout(False)
        Me.TabPage3.PerformLayout()
        Me.GroupBox9.ResumeLayout(False)
        Me.GroupBox9.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents tsMain As System.Windows.Forms.ToolStrip
    Friend WithEvents tsbSrchAll As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbSet As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbUpload As System.Windows.Forms.ToolStripButton
    Friend WithEvents ToolStripSeparator4 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents tsbExit As System.Windows.Forms.ToolStripButton
    Friend WithEvents ofd As System.Windows.Forms.OpenFileDialog
    Friend WithEvents ssMain As System.Windows.Forms.StatusStrip
    Friend WithEvents tLblState As System.Windows.Forms.ToolStripStatusLabel
    Friend WithEvents tpb As System.Windows.Forms.ToolStripProgressBar
    Friend WithEvents tlblMsg As System.Windows.Forms.ToolStripStatusLabel
    Friend WithEvents tv As System.Windows.Forms.TreeView
    Friend WithEvents Tab As System.Windows.Forms.TabControl
    Friend WithEvents TabPage1 As System.Windows.Forms.TabPage
    Friend WithEvents GroupBox5 As System.Windows.Forms.GroupBox
    Friend WithEvents cboFlow As System.Windows.Forms.ComboBox
    Friend WithEvents cboStopBit As System.Windows.Forms.ComboBox
    Friend WithEvents cboParity As System.Windows.Forms.ComboBox
    Friend WithEvents cboDataBit As System.Windows.Forms.ComboBox
    Friend WithEvents cboBaud As System.Windows.Forms.ComboBox
    Friend WithEvents Label18 As System.Windows.Forms.Label
    Friend WithEvents Label17 As System.Windows.Forms.Label
    Friend WithEvents Label16 As System.Windows.Forms.Label
    Friend WithEvents Label15 As System.Windows.Forms.Label
    Friend WithEvents Label14 As System.Windows.Forms.Label
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents Label28 As System.Windows.Forms.Label
    Friend WithEvents txtDNS As System.Windows.Forms.TextBox
    Friend WithEvents txtGw As System.Windows.Forms.TextBox
    Friend WithEvents txtSubnet As System.Windows.Forms.TextBox
    Friend WithEvents txtLIP As System.Windows.Forms.TextBox
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents rbStatic As System.Windows.Forms.RadioButton
    Friend WithEvents rbDHCP As System.Windows.Forms.RadioButton
    Friend WithEvents TabPage2 As System.Windows.Forms.TabPage
    Friend WithEvents TabPage3 As System.Windows.Forms.TabPage
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents rbTcpMixed As System.Windows.Forms.RadioButton
    Friend WithEvents rbTcpServer As System.Windows.Forms.RadioButton
    Friend WithEvents rbTcpClient As System.Windows.Forms.RadioButton
    Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents txtRhost As System.Windows.Forms.TextBox
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents txtRport As System.Windows.Forms.TextBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents txtLport As System.Windows.Forms.TextBox
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents rbUdp As System.Windows.Forms.RadioButton
    Friend WithEvents GroupBox9 As System.Windows.Forms.GroupBox
    Friend WithEvents txtReconn As System.Windows.Forms.TextBox
    Friend WithEvents txtInactivity As System.Windows.Forms.TextBox
    Friend WithEvents lblReconn As System.Windows.Forms.Label
    Friend WithEvents lblInactivity As System.Windows.Forms.Label
    Friend WithEvents txtNagle As System.Windows.Forms.TextBox
    Friend WithEvents Label24 As System.Windows.Forms.Label
    Friend WithEvents tsbFactory As System.Windows.Forms.ToolStripButton
    Friend WithEvents txtSerialNumber As System.Windows.Forms.TextBox
    Friend WithEvents txtDeviceName As System.Windows.Forms.TextBox
    Friend WithEvents Label25 As System.Windows.Forms.Label
    Friend WithEvents Label22 As System.Windows.Forms.Label
    Friend WithEvents txtPwd As System.Windows.Forms.TextBox
    Friend WithEvents Label21 As System.Windows.Forms.Label
    Friend WithEvents Timer1 As System.Windows.Forms.Timer

End Class
