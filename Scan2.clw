; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CScan2Dlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Scan2.h"

ClassCount=4
Class1=CScan2App
Class2=CScan2Dlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_SCAN2_DIALOG
Resource2=IDR_MAINFRAME
Resource3=IDD_ABOUTBOX
Class4=setport
Resource4=IDD_setport

[CLS:CScan2App]
Type=0
HeaderFile=Scan2.h
ImplementationFile=Scan2.cpp
Filter=N

[CLS:CScan2Dlg]
Type=0
HeaderFile=Scan2Dlg.h
ImplementationFile=Scan2Dlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CScan2Dlg

[CLS:CAboutDlg]
Type=0
HeaderFile=Scan2Dlg.h
ImplementationFile=Scan2Dlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_SCAN2_DIALOG]
Type=1
Class=CScan2Dlg
ControlCount=10
Control1=IDC_IP1,SysIPAddress32,1342242816
Control2=IDC_IP2,SysIPAddress32,1342242816
Control3=IDC_STATIC,static,1342308866
Control4=IDC_STATIC,static,1342308865
Control5=IDC_LIST_IP,SysListView32,1350631425
Control6=IDC_IP,button,1342242816
Control7=IDC_setport,button,1342242816
Control8=IDC_port,button,1342242816
Control9=IDC_retport,edit,1352728580
Control10=IDC_STATIC,static,1342308865

[DLG:IDD_setport]
Type=1
Class=setport
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_port1,edit,1350631552
Control4=IDC_port2,edit,1350631552
Control5=IDC_STATIC,static,1342308866
Control6=IDC_STATIC,static,1342308865

[CLS:setport]
Type=0
HeaderFile=setport.h
ImplementationFile=setport.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_port2

