Set arg = WScript.Arguments
cmd = MakeCmdLine(arg)
Set shl = CreateObject("WScript.Shell")
Call shl.Run("rundll32.exe """ & GetGdiPPFileName & """,GDIPlusPlus_RunDLL " & cmd)

Function GetGdiPPFileName()
	s = WScript.ScriptFullName
	n = InStrRev(s, "\")
	If n <> 0 Then
		GetGdiPPFileName = Left(s, n) + "gdi++.dll"
	Else
		GetGdiPPFileName = "gdi++.dll"
	End If
End Function

Function MakeCmdLine(arg)
	For i = 0 To arg.Count - 1
		If InStr(arg(i), " ") <> 0 Then
			MakeCmdLine = MakeCmdLine & """" & arg(i) & """ "
		Else
			MakeCmdLine = MakeCmdLine & arg(i) & " "
		End If
	Next
End Function
