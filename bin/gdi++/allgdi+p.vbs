Set shl = CreateObject("WScript.Shell")
Call shl.Run("rundll32.exe """ & GetGdiPPFileName & """,AllGDIPlusPlus_RunDLL -p")

Function GetGdiPPFileName()
	s = WScript.ScriptFullName
	n = InStrRev(s, "\")
	If n <> 0 Then
		GetGdiPPFileName = Left(s, n) + "gdi++.dll"
	Else
		GetGdiPPFileName = "gdi++.dll"
	End If
End Function
