Imports System
Imports System.Net
Imports System.Net.Sockets

Public Class clsSckTCP
    Public Event DataArrival(ByVal len As Integer, ByVal dat() As Byte)
    'Public Event SendData(ByVal len As Integer, ByVal dat() As Byte)
    Public Event Connected()
    Public Event Disconnected()
    Public Event SocketError(ByVal err As Exception)

    Private Class StateObject
        'work socket
        Public wSck As Socket = Nothing
        'size of receive buffer
        Public Const bufSize As Integer = 2048
        'receive buffer
        Public buf(bufSize - 1) As Byte
    End Class

    Private sck As Socket
    Private _isConnected As Boolean
    Public Property isConnected() As Boolean
        Get
            Return _isConnected
        End Get
        Set(ByVal value As Boolean)
            _isConnected = value

        End Set
    End Property
    Public Sub Connect(ByVal tcpRep As IPEndPoint)
        Try
            'begin connect
            sck = New Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp)
            If Not sck.Connected Then
                sck.BeginConnect(tcpRep, New AsyncCallback(AddressOf ConnectCallback), sck)
            End If

        Catch ex As SocketException
            RaiseEvent SocketError(ex)
        End Try


    End Sub
    Private Sub ConnectCallback(ByVal ar As IAsyncResult)
        If ar.IsCompleted Then
            Try
                'retrieve the socket from the state object
                Dim client As Socket = CType(ar.AsyncState, Socket)
                client.EndConnect(ar)
                _isConnected = True
                'raise connected event
                RaiseEvent Connected()
                'receive
                Receive()
            Catch ex As SocketException
                RaiseEvent SocketError(ex)
            End Try

        End If
    End Sub
    Public Sub Disconnect()
        Try
            'begin disconnect
            'sck.Shutdown(SocketShutdown.Both)
            sck.BeginDisconnect(True, New AsyncCallback(AddressOf DisconnectCallback), sck)
            _isConnected = False

        Catch ex As SocketException

            RaiseEvent SocketError(ex)
        End Try

    End Sub
    Private Sub DisconnectCallback(ByVal ar As IAsyncResult)
        If ar.IsCompleted Then
            'Dim client = CType(ar.AsyncState, Socket)
            'sck = CType(ar.AsyncState, Socket)
            Try
                sck.EndDisconnect(ar)
                'sck.EndDisconnect(ar)
                'raise disconnected event
                RaiseEvent Disconnected()

            Catch ex As Exception

                RaiseEvent SocketError(ex)
            End Try
        End If
    End Sub
    Private Sub Receive()
        Try
            'create the state object
            Dim state As New StateObject
            state.wSck = sck
            sck.BeginReceive(state.buf, 0, StateObject.bufSize, 0, New AsyncCallback(AddressOf ReceiveCallback), state)
        Catch ex As Exception
            RaiseEvent SocketError(ex)
        End Try
    End Sub
    Private Sub ReceiveCallback(ByVal ar As IAsyncResult)
        If ar.IsCompleted Then
            Dim state As StateObject = CType(ar.AsyncState, StateObject)
            sck = state.wSck

            Dim bytesRead As Integer = 0

            Try
                bytesRead = sck.EndReceive(ar)
                If bytesRead > 0 Then
                    'raise DataArrival event
                    Dim dat(bytesRead - 1) As Byte
                    Array.ConstrainedCopy(state.buf, 0, dat, 0, bytesRead)
                    RaiseEvent DataArrival(bytesRead, dat)
                    'receive again
                    sck.BeginReceive(state.buf, 0, StateObject.bufSize, 0, New AsyncCallback(AddressOf ReceiveCallback), state)
                Else
                    Call Disconnect()
                End If
                
            Catch ex As Exception
                'raise error event
                RaiseEvent SocketError(ex)
            End Try

        End If

    End Sub
    Public Sub Send(ByVal len As Integer, ByVal dat() As Byte)
        Try
            'begin sending the data
            sck.BeginSend(dat, 0, len, SocketFlags.None, New AsyncCallback(AddressOf SendCallback), sck)
        Catch ex As Exception
            RaiseEvent SocketError(ex)
        End Try

    End Sub
    Private Sub SendCallback(ByVal ar As IAsyncResult)
        If ar.IsCompleted Then
            Dim sck As Socket = CType(ar.AsyncState, Socket)
            Try
                Dim bytesSent As Integer = 0
                bytesSent = sck.EndSend(ar)
            Catch ex As Exception
                RaiseEvent SocketError(ex)
            End Try
        End If

    End Sub
End Class

