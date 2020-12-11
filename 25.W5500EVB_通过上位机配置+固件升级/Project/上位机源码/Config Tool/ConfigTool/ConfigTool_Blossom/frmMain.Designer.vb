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
        Me.components = New System.ComponentModel.Container()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmMain))
        Me.tsMain = New System.Windows.Forms.ToolStrip()
        Me.tsbSrchAll = New System.Windows.Forms.ToolStripButton()
        Me.tsbSet = New System.Windows.Forms.ToolStripButton()
        Me.tsbUpload = New System.Windows.Forms.ToolStripButton()
        Me.tsbReset = New System.Windows.Forms.ToolStripButton()
        Me.ToolStripSeparator4 = New System.Windows.Forms.ToolStripSeparator()
        Me.tsbExit = New System.Windows.Forms.ToolStripButton()
        Me.ofd = New System.Windows.Forms.OpenFileDialog()
        Me.ssMain = New System.Windows.Forms.StatusStrip()
        Me.tLblState = New System.Windows.Forms.ToolStripStatusLabel()
        Me.tpb = New System.Windows.Forms.ToolStripProgressBar()
        Me.tlblMsg = New System.Windows.Forms.ToolStripStatusLabel()
        Me.tv = New System.Windows.Forms.TreeView()
        Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
        Me.txtDNS = New System.Windows.Forms.TextBox()
        Me.txtGw = New System.Windows.Forms.TextBox()
        Me.txtSubnet = New System.Windows.Forms.TextBox()
        Me.txtLIP = New System.Windows.Forms.TextBox()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.txtMac = New System.Windows.Forms.TextBox()
        Me.txtVer = New System.Windows.Forms.TextBox()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.tsMain.SuspendLayout()
        Me.ssMain.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'tsMain
        '
        Me.tsMain.ImageScalingSize = New System.Drawing.Size(32, 32)
        Me.tsMain.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.tsbSrchAll, Me.tsbSet, Me.tsbUpload, Me.tsbReset, Me.ToolStripSeparator4, Me.tsbExit})
        Me.tsMain.Location = New System.Drawing.Point(0, 0)
        Me.tsMain.Name = "tsMain"
        Me.tsMain.RenderMode = System.Windows.Forms.ToolStripRenderMode.System
        Me.tsMain.Size = New System.Drawing.Size(480, 39)
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
        'tsbReset
        '
        Me.tsbReset.Image = CType(resources.GetObject("tsbReset.Image"), System.Drawing.Image)
        Me.tsbReset.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbReset.Name = "tsbReset"
        Me.tsbReset.Size = New System.Drawing.Size(76, 36)
        Me.tsbReset.Text = "Reset"
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
        Me.ssMain.Location = New System.Drawing.Point(0, 266)
        Me.ssMain.Name = "ssMain"
        Me.ssMain.Padding = New System.Windows.Forms.Padding(1, 0, 12, 0)
        Me.ssMain.Size = New System.Drawing.Size(480, 22)
        Me.ssMain.TabIndex = 14
        '
        'tLblState
        '
        Me.tLblState.Image = CType(resources.GetObject("tLblState.Image"), System.Drawing.Image)
        Me.tLblState.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None
        Me.tLblState.Name = "tLblState"
        Me.tLblState.Size = New System.Drawing.Size(88, 17)
        Me.tLblState.Text = "ConfigTool"
        '
        'tpb
        '
        Me.tpb.Name = "tpb"
        Me.tpb.Size = New System.Drawing.Size(129, 16)
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
        Me.tv.Location = New System.Drawing.Point(4, 45)
        Me.tv.Name = "tv"
        Me.tv.ShowNodeToolTips = True
        Me.tv.Size = New System.Drawing.Size(199, 215)
        Me.tv.TabIndex = 15
        '
        'Timer1
        '
        '
        'txtDNS
        '
        Me.txtDNS.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtDNS.Location = New System.Drawing.Point(138, 185)
        Me.txtDNS.Name = "txtDNS"
        Me.txtDNS.Size = New System.Drawing.Size(113, 21)
        Me.txtDNS.TabIndex = 29
        '
        'txtGw
        '
        Me.txtGw.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtGw.Location = New System.Drawing.Point(138, 131)
        Me.txtGw.Name = "txtGw"
        Me.txtGw.Size = New System.Drawing.Size(113, 21)
        Me.txtGw.TabIndex = 28
        '
        'txtSubnet
        '
        Me.txtSubnet.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtSubnet.Location = New System.Drawing.Point(138, 158)
        Me.txtSubnet.Name = "txtSubnet"
        Me.txtSubnet.Size = New System.Drawing.Size(113, 21)
        Me.txtSubnet.TabIndex = 27
        '
        'txtLIP
        '
        Me.txtLIP.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtLIP.Location = New System.Drawing.Point(138, 104)
        Me.txtLIP.Name = "txtLIP"
        Me.txtLIP.Size = New System.Drawing.Size(113, 21)
        Me.txtLIP.TabIndex = 26
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Location = New System.Drawing.Point(61, 190)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(71, 12)
        Me.Label4.TabIndex = 25
        Me.Label4.Text = "DNS server:"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(31, 163)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(101, 12)
        Me.Label3.TabIndex = 24
        Me.Label3.Text = "Default gateway:"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(55, 136)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(77, 12)
        Me.Label2.TabIndex = 23
        Me.Label2.Text = "Subnet mask:"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(19, 109)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(113, 12)
        Me.Label1.TabIndex = 22
        Me.Label1.Text = "Device IP address:"
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.txtMac)
        Me.GroupBox1.Controls.Add(Me.txtVer)
        Me.GroupBox1.Controls.Add(Me.Label6)
        Me.GroupBox1.Controls.Add(Me.Label5)
        Me.GroupBox1.Controls.Add(Me.txtLIP)
        Me.GroupBox1.Controls.Add(Me.txtDNS)
        Me.GroupBox1.Controls.Add(Me.Label1)
        Me.GroupBox1.Controls.Add(Me.txtGw)
        Me.GroupBox1.Controls.Add(Me.Label2)
        Me.GroupBox1.Controls.Add(Me.txtSubnet)
        Me.GroupBox1.Controls.Add(Me.Label3)
        Me.GroupBox1.Controls.Add(Me.Label4)
        Me.GroupBox1.Location = New System.Drawing.Point(208, 41)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(266, 219)
        Me.GroupBox1.TabIndex = 30
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "Device settings"
        '
        'txtMac
        '
        Me.txtMac.Location = New System.Drawing.Point(138, 54)
        Me.txtMac.Name = "txtMac"
        Me.txtMac.ReadOnly = True
        Me.txtMac.Size = New System.Drawing.Size(113, 21)
        Me.txtMac.TabIndex = 33
        '
        'txtVer
        '
        Me.txtVer.Location = New System.Drawing.Point(138, 27)
        Me.txtVer.Name = "txtVer"
        Me.txtVer.ReadOnly = True
        Me.txtVer.Size = New System.Drawing.Size(113, 21)
        Me.txtVer.TabIndex = 32
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Location = New System.Drawing.Point(25, 30)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(107, 12)
        Me.Label6.TabIndex = 31
        Me.Label6.Text = "Firmware version:"
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(55, 57)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(77, 12)
        Me.Label5.TabIndex = 30
        Me.Label5.Text = "MAC address:"
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(480, 288)
        Me.Controls.Add(Me.GroupBox1)
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
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
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
    Friend WithEvents tsbReset As System.Windows.Forms.ToolStripButton
    Friend WithEvents Timer1 As System.Windows.Forms.Timer
    Friend WithEvents txtDNS As System.Windows.Forms.TextBox
    Friend WithEvents txtGw As System.Windows.Forms.TextBox
    Friend WithEvents txtSubnet As System.Windows.Forms.TextBox
    Friend WithEvents txtLIP As System.Windows.Forms.TextBox
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents txtMac As System.Windows.Forms.TextBox
    Friend WithEvents txtVer As System.Windows.Forms.TextBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label

End Class
