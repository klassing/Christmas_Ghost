'---------------------------------------------------------------------------------------------
'---------------------------------------------------------------------------------------------
'----
'----			Author: Ryan Klassing  
'----
'----			
'----			Version 1.00
'----			
'----			Description:
'----			The intent of this script is to create a new library directory, pre-configured with
'----			the necessary .h and .cpp files to allow the user to add lighting functions that are
'----			protected against naming conflicts with other users.
'----
'----           Expected Use:
'----           User should 'double click' this script, follow the windows prompts, and the directory
'----           and necessary lib files will be automatically created.  The user is then free to edit
'----           these new lib files as they see fit!
'----
'---------------------------------------------------------------------------------------------
'---------------------------------------------------------------------------------------------
'----
'----		  ~~Version History~~
'----
'----		  	Version 1.00 -
'----			Initial launch of the script.
'----
'----------------------------------------------------------------------------------------------
'----------------------------------------------------------------------------------------------

option explicit

'Declare global variables
Dim objFSO								'File system object
Dim objShell
Dim rExp
Dim strClassName
Dim output_lib_folder
Dim output_src_folder
Dim header_file
Dim strHeader_File
Dim cpp_file
Dim strCPP_File
Dim readme_file
Dim strReadme_File
Dim temp_file

'Define global constants
Const ForReading = 1
Const ForWriting = 2
Const ForAppending = 8
Const head_ext = ".h"
Const cpp_ext = ".cpp"
Const readme_name = "README.md"
Const template_head_file = "\lib_template\lib_template.h"
Const template_cpp_file = "\lib_template\lib_template.cpp"
Const template_readme_file = "\lib_template\README.md"
Const destination_folder = "\lib\"
Const default_class_name = "[CLASS_NAME]"
Const default_class_guard = "[CLASS_NAME_DEFINE]"
Const default_date = "[TODAY]"

Call main()

Sub main()
	'Init the scripting object
	Set objFSO = CreateObject("Scripting.FileSystemObject")

	'Prepare the regular expression for checking alpha-numerics'
	Set rExp = New RegExp
	rExp.Pattern = "[^a-zA-Z0-9]"

	'Prompt the user for the desired class name
	strClassName = InputBox("Please enter a name for the new user class." + vbCrLf + "[ex: your last name]" + vbCrLf _ 
	+ "(Please avoid non alpha-numberic characters.)", "Enter Class Name")

	'Verify something was provided and is alpha-numeric
	If strClassName = "" Then
		Wscript.Echo "User cancelled the script, no library was created."
		Wscript.Quit
	ElseIf rExp.Test(strClassName) Then
		Wscript.Echo "Found a non alpha-numeric character in " + Chr(39) + strClassName + Chr(39) +  vbCrLf + "Please try again, but follow the rules this time :-)."
		Wscript.Quit
	End If

	'Set the target directory for where we plan to write the lib files
	output_lib_folder = RemoveDoubleSlash(objFSO.GetParentFolderName(objFSO.GetParentFolderName(WScript.ScriptFullName)) + "\" + destination_folder + "\" + strClassName + "\")
	
	'Check if the output directory exists
	If objFSO.FolderExists(output_lib_folder) Then
		Wscript.Echo "!! Error !!" + vbCrLf + vbCrLf + Chr(39) + strClassName + Chr(39) + " lib folder already exists: " + vbCrLf +vbCrLf + output_lib_folder + vbCrLf + vbCrLf _
		+ "Please do one of the following:" + vbCrLf _
		+ vbTab + "1) Pick a new Class Name" + vbCrLf _
		+ vbTab + "2) Delete the conflicting lib class folder (not recommended)" + vbCrLf _
		+ vbTab + "3) Rename the conflicting lib class (not recommended)"

		Wscript.Quit
	End If

	'Open the header template file
	temp_file = RemoveDoubleSlash(objFSO.GetParentFolderName(WScript.ScriptFullName) + "\" + template_head_file)
	If objFSO.FileExists(temp_file) Then
		Set header_file = objFSO.OpenTextFile(temp_file, ForReading)
		strHeader_File = header_file.ReadAll
		header_file.Close
		Set header_file = Nothing
	Else
		Wscript.Echo "!! Error !!" + vbCrLf + vbCrLf + "Header (.h) file template is missing from: " + vbCrLf + temp_file _
			+ vbCrLf + vbCrLf + "Please contact Ryan Klassing for help"
		Wscript.Quit
	End If

	'Open the cpp template file
	temp_file = RemoveDoubleSlash(objFSO.GetParentFolderName(WScript.ScriptFullName) + "\" + template_cpp_file)
	If objFSO.FileExists(temp_file) Then
		Set cpp_file = objFSO.OpenTextFile(temp_file, ForReading)
		strCPP_File = cpp_file.ReadAll
		cpp_file.Close
		Set cpp_file = Nothing
	Else
		Wscript.Echo "!! Error !!" + vbCrLf + vbCrLf + "Source (.cpp) file template is missing from: " + vbCrLf + temp_file _
			+ vbCrLf + vbCrLf + "Please contact Ryan Klassing for help"
		Wscript.Quit
	End If

	'Open the readme template file
	temp_file = RemoveDoubleSlash(objFSO.GetParentFolderName(WScript.ScriptFullName) + "\" + template_readme_file)
	If objFSO.FileExists(temp_file) Then
		Set readme_file = objFSO.OpenTextFile(temp_file, ForReading)
		strReadme_File = readme_file.ReadAll
		readme_file.Close
		Set readme_file = Nothing
	Else
		Wscript.Echo "!! Error !!" + vbCrLf + vbCrLf + "Readme (README.md) file template is missing from: " + vbCrLf + temp_file _
			+ vbCrLf + vbCrLf + "Please contact Ryan Klassing for help"
		Wscript.Quit
	End If

	'In the header file --> replace the #define guard, the class name, and today's date
	strHeader_File = Replace(Replace(Replace(strHeader_File, default_class_name, strClassName), default_class_guard, strClassName + "_h"), default_date, date())

	'In the cpp file --> replace the class name
	strCPP_File = Replace(Replace(strCPP_File, default_class_name, strClassName), default_date, date())

	'In the README file --> replace the class name
	strReadme_File = Replace(strReadme_File, default_class_name, strClassName)

	'Create the src directory if it doesn't yet exist
	output_src_folder = RemoveDoubleSlash(output_lib_folder + "\src\")
	BuildFullPath(output_src_folder)

	'Write the header file
	Set header_file = objFSO.OpenTextFile(RemoveDoubleSlash(output_src_folder + "\" + strClassName + head_ext), ForWriting, True)
	header_file.WriteLine(strHeader_File)
	header_file.Close
	Set header_file = Nothing

	'Write the cpp file
	Set cpp_file = objFSO.OpenTextFile(RemoveDoubleSlash(output_src_folder + "\" + strClassName + cpp_ext), ForWriting, True)
	cpp_file.WriteLine(strCPP_File)
	cpp_file.Close
	Set cpp_file = Nothing

	'Write the Readme file in the lib directory (created automatically above when we created the src directory)
	Set readme_file = objFSO.OpenTextFile(RemoveDoubleSlash(output_lib_folder + "\" + readme_name), ForWriting, True)
	readme_file.WriteLine(strReadme_File)
	readme_file.Close
	Set readme_file = Nothing

	'Inform the user
	Wscript.Echo "New lib " + Chr(39) + strClassName + Chr(39) + " successfully created at: " + vbCrLf + vbCrLf + output_lib_folder

	'Open a windows explorer for the user to this new directory
	Set objShell = CreateObject("Wscript.Shell")
	objShell.Run "explorer.exe /e," + output_lib_folder

	'Clean up
	Set objShell = Nothing
	Set objFSO = Nothing

End Sub

Sub BuildFullPath(ByVal FullPath)
    '---------------------------------------------------------------------------------------------
    '----   Description:
    '----   The intent of this routine is to build a filepath, even if its parent directories dont' exist
    '----
    '----   Input:
    '----       FullPath = string for the full path of the desired folder structure to be created
    '---------------------------------------------------------------------------------------------

    If Not objFSO.FolderExists(FullPath) Then
        BuildFullPath objFSO.GetParentFolderName(FullPath)
        objFSO.CreateFolder FullPath
    End If
End Sub

Public Function RemoveDoubleSlash(ByVal filepath)
    '---------------------------------------------------------------------------------------------
    '----   Description:
    '----   The intent of this function is to remove unintentional double slashes (\\) from a filepath.
    '----       The script will identify UNC filepaths (which start with "\\") and will not remove those.
    '----
    '----   Input:
    '----       FilePath = filepath (as a string) for which to remove double slashes
    '----
    '----   Return:
    '----       RemoveDoubleSlash = filepath, but with "\\" replaced with "\"
    '---------------------------------------------------------------------------------------------
    
    '------Variable Definitions------
        Dim AddUNC
    
    '------Initialize Variables------
        AddUNC = False
    
    '--------Actual Function---------
        'Check if filepath is UNC
        If Left(filepath, 2) = "\\" Then
            AddUNC = True
            filepath = Right(filepath, Len(filepath) - 2)
        End If
        
        'Call recursively until all "\\" are replaced with "\"
        filepath = Replace(filepath, "\\", "\", 1)
        If InStr(1, filepath, "\\") > 0 Then filepath = RemoveDoubleSlash(filepath)
        
        'if filepath was UNC, add the leading "\\" back now
        If AddUNC Then filepath = "\\" & filepath
        
        'Return
        RemoveDoubleSlash = filepath
End Function