Set l_Selection = Application.Selection(0)
If TypeName(l_Selection) <> "Nothing" Then
	For Each mat in l_Selection.Materials
    	SIApplyShaderToCnxPoint "RealTime\GamebryoFX", mat.RealTime
    Next
End If
