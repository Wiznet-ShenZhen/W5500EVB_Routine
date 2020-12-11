
/**
******************************************************************************
* @file   webpge.h
* @author  WIZnet Software Team 
* @version V1.0
* @date    2015-xx-xx
* @brief   web服务器现在的内容
* @attention  
******************************************************************************
**/
#ifndef __WEBPAGE_H
#define __WEBPAGE_H
#define INDEX_HTML  "<!DOCTYPE html>"\
"<html>"\
"<head>"\
"<meta http-equiv='Content-Type' content='text/html; charset=GB2312'/>"\
"<title>W5500EVB::梦想版</title>"\
"<style>"\
"::selection{background:red;color:#fff;}"\
"::-moz-selection{background:red;color:#fff;}"\
"::-webkit-selection{background:red;color:#fff;}"\
"body,div,h1,h2,h3{margin: 0; padding: 0; border: 0;}"\
"body{background: #474747;text-align:center;}"\
"#inset h1{"\
"text-align: center;"\
"margin: 0px auto;"\
"font-family:'黑体','宋体';"\
"font-size: 100px; text-transform: uppercase;"\
"color: #222;"\
"text-shadow: 0px 2px 3px #666;"\
"display:block;}"\
"h1 a:link{text-decoration:none; background:red;color:#fff}"\
"h1 a:hover{color: #fff; text-shadow: 0px 5px 8px #2a2a2a; text-decoration:none;}"\
"h1 a:visited{color:#fff;}"\
"#inset{margin-top:150px;margin-right:auto;margin-left:auto;}"\
".foot{margin-top:20px;font-size:10px;font-family:'arial';color:#222;}"\
"</style>"\
"</head>"\
"<body>"\
"<div id='inset'>"\
"<h1><a href='dream.html' target='_blank'>梦想</a></h1>"\
"<h1>还是要有的</h1>"\
"<h1><a href='config.html' target='_blank'>万一</a></h1>"\
"<h1>实现了呢</h1>"\
"<h3 class='foot'>&copy;2016 WIZnet Shenzhen Team</h3>"\
"</div>"\
"</body>"\
"</html>"\

#define DREAM_HTML  "<!DOCTYPE html>"\
"<html>"\
"<head>"\
"<title>W5500EVB::例程下载</title>"\
"<style type='text/css'>"\
"div{float:left;margin:0 260px;}"\
"</style>"\
"</head>"\
"<body>"\
"<div>"\
"<table>"\
"<table border='2'>"\
"<caption>"\
"<h1>W5500EVB例程下载</h1>"\
"</caption>"\
"<br></br>"\
"<tr>"\
"<th>例程名称</th>"\
"<th>功&nbsp;&nbsp;能</th>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/1.Network Install/1.Network Install.zip' style='text-decoration:none'>&nbsp;&lt;1.Network Install&gt; </a></td>"\
"<td>&nbsp;网络初始化配置，可以通过计算机Ping W5500EVB进行测试</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/2.TCPS/2.TCPS.zip' style='text-decoration:none'>&nbsp;&lt;2.TCP Server&gt; </a></td>"\
"<td>&nbsp;W5500EVB作为TCP服务器，同远程设备TCP客户端进行通信</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/3.TCPC/3.TCPC.zip' style='text-decoration:none'>&nbsp;&lt;3.TCP Client&gt; </a></td>"\
"<td>&nbsp;W5500EVB作为TCP客户端，同远程设备TCP服务器进行通信</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/4.UDP/4.UDP.zip' style='text-decoration:none'>&nbsp;&lt;4.UDP&gt; </a></td>"\
"<td>&nbsp;W5500EVB同远程设备之间通过UDP协议收/发数据</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/5.DHCP/5.DHCP.zip' style='text-decoration:none'>&nbsp;&lt;5.DHCP&gt; </a></td>"\
"<td>&nbsp;W5500EVB从路由器通过DHCP方式获取IP</td>"\
"</tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/6.DNS/6.DNS.zip' style='text-decoration:none'>&nbsp;&lt;6.DNS&gt; </a></td>"\
"<td>&nbsp;W5500EVB通过DNS（域名系统）将用户输入的域名解析为对应的IP地址</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/7.HTTPS/7.HTTPS.zip' style='text-decoration:none'>&nbsp;&lt;7.HTTP Server&gt; </a></td>"\
"<td>&nbsp;W5500EVB作为HTTP服务器，用户可以通过网页对W5500EVB进行配置</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/8.HTTPC/8.HTTPC.zip' style='text-decoration:none'>&nbsp;&lt;8.HTTP Client&gt; </a></td>"\
"<td>&nbsp;W5500EVB作为HTTP客户端，向Yeelink云平台提交采集到的数据</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/9.NTP/9.NTP.zip' style='text-decoration:none'>&nbsp;&lt;9.NTP&gt; </a></td>"\
"<td>&nbsp;W5500EVB从网络上获取标准时间</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/10.SMTP/10.SMTP.zip' style='text-decoration:none'>&nbsp;&lt;10.SMTP&gt; </a></td>"\
"<td>&nbsp;W5500EVB向用户指定的邮箱发送邮件</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/11.Weibo/11.Weibo.zip' style='text-decoration:none'>&nbsp;&lt;11.Weibo&gt; </a></td>"\
"<td>&nbsp;W5500EVB实现发微博功能（目前微博服务器故障，该例程仅供参考）</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/12.NetBIOS/12.NetBIOS.zip' style='text-decoration:none'>&nbsp;&lt;12.NetBIOS&gt; </a></td>"\
"<td>&nbsp;NetBIOS协议演示，用户在浏览器中输入“W5500EVB”即可登录进行网页配置&nbsp;&nbsp;&nbsp;</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/13.UPnP/13.UPnP.zip' style='text-decoration:none'>&nbsp;&lt;13.UPnP&gt; </a></td>"\
"<td>&nbsp;UPnP协议演示</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/14.TFTPC/14.TFTPC.zip' style='text-decoration:none'>&nbsp;&lt;14.TFTP Client&gt; </a></td>"\
"<td>&nbsp;TFTP协议演示，W5500EVB同计算机之间进行文件传输</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/15.SNMP/15.SNMP.zip' style='text-decoration:none'>&nbsp;&lt;15.SNMP&gt; </a></td>"\
"<td>&nbsp;SNMP协议V1版本演示</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/16.Ping/16.Ping.zip' style='text-decoration:none'>&nbsp;&lt;16.Ping&gt; </a></td>"\
"<td>&nbsp;Ping例程演示，W5500EVB Ping计算机</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/W5500EVB N合1例程/W5500EVB N合1例程.zip' style='text-decoration:none'>&nbsp;&lt;17.W5500EVB N合1例程&gt;&nbsp;</a></td>"\
"<td>&nbsp;将以上例程整合为1个总例程，方便用户体验</td>"\
"</tr>"\
"<tr>"\
"<td><a href='http://www.w5500.com/download/code/w5500evb/W5500EVB例程（全）/W5500EVB例程(全).zip' style='text-decoration:none'>&nbsp;&lt;18.W5500EVB例程(全)&gt; </a></td>"\
"<td>&nbsp;将以上17个例程打包下载</td>"\
"</tr>"\
"</table>"\
"<br></br>"\
"<div align='center'>&copy; 2016 WIZnet Shenzhen Team"\
"<br></br>"\
"</div>"\
"</body>"\
"</html>"\


#define CONFIG_HTML  "<!DOCTYPE html>"\
"<html>"\
"<head>"\
"<title>W5500EVB::网页配置</title>"\
"<meta http-equiv='Content-Type' content='text/html; charset=GB2312'/>"\
"<style type='text/css'>"\
"body {text-align:left; background-color:#c0deed;font-family:Verdana;}"\
"#main {margin-right:auto;margin-left:auto;margin-top:30px;}"\
"label{display:inline-block;width:150px;}"\
"#main h3{color:#66b3ff; text-decoration:underline;}"\
"</style>"\
"<script>"\
"function $(id) { return document.getElementById(id); };"\
"function settingsCallback(o) {"\
"if ($('txtVer')) $('txtVer').value = o.ver;"\
"if ($('txtMac')) $('txtMac').value = o.mac;"\
"if ($('txtIp')) $('txtIp').value = o.ip;"\
"if ($('txtSub')) $('txtSub').value = o.sub;"\
"if ($('txtGw')) $('txtGw').value = o.gw;"\
"};"\
"</script>"\
"</head>"\
"<body>"\
"<div id='main'>"\
"<div style='background:snow; display:block;padding:10px 20px;'>"\
"<h3>配置网络参数</h3>"\
"<form id='frmSetting' method='POST' action='config.cgi'>"\
"<p><label for='txtIp'>固件版本号:</label><input type='text' id='txtVer' name='ver' size='16' disabled='disabled' /></p>"\
"<p><label for='txtIp'>MAC地址:</label><input type='text' id='txtMac' name='mac' size='16' disabled='disabled' /></p>"\
"<p><label for='txtIp'>IP地址:</label><input type='text' id='txtIp' name='ip' size='16' /></p>"\
"<p><label for='txtSub'>子网掩码:</label><input type='text' id='txtSub' name='sub' size='16' /></p>"\
"<p><label for='txtGw'>默认网关:</label><input type='text' id='txtGw' name='gw' size='16' /></p>"\
"<p><input type='submit' value='保存并重启' /></p>"\
"</form>"\
"</div>"\
"</div>"\
"<div style='margin:5px 5px;'>"\
"&copy; 2016 WIZnet Shenzhen Team"\
"</div>"\
"<script type='text/javascript' src='w5500.js'></script>"\
"</body>"\
"</html>"

#endif
