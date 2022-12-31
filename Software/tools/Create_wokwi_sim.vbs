'---------------------------------------------------------------------------------------------
'---------------------------------------------------------------------------------------------
'----
'----			Author: Ryan Klassing  
'----
'----			
'----			Version 1.00
'----			
'----			Description:
'----			The intent of this script is to convert the current development into a single file
'----			that can be copy/pasted into the free online arduino simulator known as 'wokwi':
'----			https://wokwi.com/projects/352480708315963393
'----
'----			Note: this is a 'workaround' to allow more complex developments (multi-file)
'----			to still use the 'free' simulator (the paid simulator allows custom library uploads
'----			and would be much simpler, since it wouldn't require code manipulation to run).
'----
'----           Expected Use:
'----           User should 'double click' this script, which will create a single 'main.ino' file
'----			that can be copy/pasted into the wokwi simulation.
'----
'----			Note: the script will also put the full content onto the user's clipboard for convenience.
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
Dim objFSO							'File system object
Dim objShell
Dim objFolder
Dim software_folder					'Filepath: parent "Software" folder
Dim src_folder						'Filepath: main src folder
Dim lib_folder						'Filepath: lib folder
Dim output_folder					'Filepath: where we'll write the output file
Dim MainFileInPath					'Filepath: where we'll find the main.cpp input file
Dim strMainFile						'String: contents of Main File
Dim MainFileIn						'File object: Main File Input
Dim MainFileOut						'File object: Main File Output
Dim strTempFile						'String: string that will contain all concatenated content as we go
Dim TempFile						'File object: Temporary File (used for reading in content)

'Declare array of libraries that must be found in a particular order
Dim primary_libraries
primary_libraries = Array("lightTools")
Dim primary_index

Dim lib_array(0)
Dim lib_index

'Define global constants
Const ForReading = 1
Const ForWriting = 2
Const ForAppending = 8
Const head_ext = ".h"
Const cpp_ext = ".cpp"
Const lib_dir = "\lib\"
Const src_dir = "\src\"
Const output_dir = "\temp\"
Const MainFileInName = "main.cpp"
Const MainFileOutName = "main.ino"
Const main_file_insert_tag = "//[INSERT_PRE-COMPILE_HERE]"
Const main_file_simulation_tag_search = "//#define ONLINE_SIMULATION"
Const main_file_simulation_tag_replace = "#define ONLINE_SIMULATION"

'Initialize variables
strTempFile = ""

Call main()

Sub main()
	'Init the scripting object
	Set objFSO = CreateObject("Scripting.FileSystemObject")

	'Setup the various folders that we'll need information from
	software_folder = RemoveDoubleSlash(objFSO.GetParentFolderName(objFSO.GetParentFolderName(WScript.ScriptFullName)) + "\")
	src_folder = RemoveDoubleSlash(software_folder + "\" + src_dir)
	MainFileInPath = RemoveDoubleSlash(src_folder + "\" + MainFileInName)
	lib_folder = RemoveDoubleSlash(software_folder + "\" + lib_dir)
	output_folder = RemoveDoubleSlash(software_folder + "\" + output_dir)

	'Verify all necessary input folders were found
	If Not objFSO.FolderExists(software_folder) or Not objFSO.FolderExists(src_folder) or Not objFSO.FolderExists(lib_folder) or Not objFSO.FileExists(MainFileInPath) Then
		Wscript.echo "Unable to find critical directories.  Please make sure the following directories exist and re-run the script:" + vbCrLf + vbCrLf + _
		"   Software\ : " + software_folder + vbCrLf + vbCrLf + _
		"   src\ : " + src_folder + vbCrLf + vbCrLf + _
		"   lib\ : " + lib_folder + vbCrLf + vbCrLf + _
		"   main.cpp : " + MainFileInPath

		Wscript.Quit
	End If

	Dim primary_index
	Dim libSrcFolder
	Dim libRootFolder
	Dim searchFolder

	For primary_index = lbound(primary_libraries) to ubound(primary_libraries)
		libRootFolder = RemoveDoubleSlash(lib_folder + "\" + primary_libraries(primary_index) + "\")
		libSrcFolder = RemoveDoubleSlash(libRootFolder + "\src\")

		'If the src folder exists, search there.  Otherwise, search the root folder
		If objFSO.FolderExists(libSrcFolder) Then
			searchFolder = libSrcFolder
		ElseIf objFSO.FolderExists(libRootFolder) Then
			searchFolder = libRootFolder
		Else
			searchFolder = ""
		End If

		If len(searchFolder) > 0 Then
			'Find all .h files in the searchFolder and add their contents to the concatenated string
			Call AddFileContent(strTempFile, searchFolder, head_ext)

			'Find all .cpp files in the searchFolder and add their contents to the concatenated string
			Call AddFileContent(strTempFile, searchFolder, cpp_ext)
		End If
	Next 'primary_index

	'Now loop through all other folders, skipping the primary_libraries folders (already added by above)
	For Each objFolder in objFSO.GetFolder(lib_folder).SubFolders
		Dim skipFolder
		skipFolder = false
		'Make sure this isn't a primary library, since those are already added
		For primary_index = lbound(primary_libraries) to ubound(primary_libraries)
			If LCase(objFolder.Name) = LCase(primary_libraries(primary_index)) Then skipFolder = true
		Next 'primary_index

		If Not skipFolder Then
			libSrcFolder = RemoveDoubleSlash(objFolder + "\src\")

			'If the src folder exists, search there.  Otherwise, search the root folder'
			If objFSO.FolderExists(libSrcFolder) Then 
				searchFolder = libSrcFolder
			Else 
				searchFolder = objFolder
			End If

			'Find all .h files in the searchFolder and add their contents to the concatenated string
			Call AddFileContent(strTempFile, searchFolder, head_ext)

			'Find all .cpp files in the searchFolder and add their contents to the concatenated string
			Call AddFileContent(strTempFile, searchFolder, cpp_ext)
		End If
	Next 'objFolder

	'Open the main.cpp file
	Set MainFileIn = objFSO.OpenTextFile(MainFileInPath, ForReading)
	strMainFile = MainFileIn.ReadAll
	MainFileIn.Close
	Set MainFileIn = Nothing

	'Add a final space at the end of the concatenated text, for good measure
	strTempFile = strTempFile + vbCrLf

	'Write the concatenated string to the main file string
	strMainFile = Replace(Replace(strMainFile, main_file_insert_tag, strTempFile),main_file_simulation_tag_search,main_file_simulation_tag_replace)

	'Create the output directory if it doesn't yet exist
	BuildFullPath(output_folder)

	'Write the new main file (overwriting if one exists)
	Set MainFileOut = objFSO.OpenTextFile(RemoveDoubleSlash(output_folder + "\" + MainFileOutName), ForWriting, True)
	MainFileOut.WriteLine(strMainFile)
	MainFileOut.Close
	Set MainFileOut = Nothing

	'Add the contents of the MainFileOut to the user's clipboard, for direct pasting if desired
	Dim WshShell
	Dim oExec
	Dim oIn
	Const sleepTime = 100		'100ms
	Const sleepTimeout = 10000	'10000ms
	Dim sleepCheck

	Set WshShell = CreateObject("WScript.Shell")
	Set oExec = WshShell.Exec("clip")
	Set oIn = oExec.stdIn
	oIn.WriteLine strMainFile
	oIn.Close

	'Wait for the clip to be finished, but timeout after 10 seconds
	sleepCheck = 0
	Do While oExec.Status = 0 And sleepCheck < sleepTimeout
		sleepCheck = sleepCheck + sleepTime
		Wscript.Sleep sleepTime
	Loop

	'Almost finished --> see if we had a timeout or not
	Dim timeout
	Dim finished_message
	If sleepCheck < sleepTimeout Then timeout = false

	'Prepare the message for the user
	finished_message = "Script completed and concatenated file written to:" + vbCrLf + vbCrLf + RemoveDoubleSlash(output_folder + "\" + MainFileOutName)

	If Not timeout Then
		finished_message = finished_message + vbCrLf + vbCrLf + "Concatenated file contents were copied to the windows clipboard." + vbCrLf + "Direct pasting (ctrl + v) should be ready!"
	End If

	Wscript.echo finished_message

	'Clean up
	Set oExec = Nothing
	Set oIn = Nothing
	Set objShell = Nothing
	Set objFSO = Nothing

End Sub

Sub AddFileContent(ByRef strConcat, ByVal folderPath, ByVal fileExtension)
    '---------------------------------------------------------------------------------------------
    '----   Description:
    '----   The intent of this routine is to find any files (with the fileExtension, inside the folderPath) and add their contents to the end of strConcat
    '---------------------------------------------------------------------------------------------
	Dim searchFile
	For Each searchFile in objFSO.GetFolder(folderPath).Files
		If LCase(Right(searchFile.Name,Len(fileExtension))) = LCase(fileExtension) Then
			'Found a matching file -> open it and read all contents into the strConcat
			Set TempFile = objFSO.OpenTextFile(searchFile.path, ForReading, True)
			strConcat = strConcat + vbCrLf + TempFile.ReadAll
			TempFile.Close
			Set TempFile = Nothing
		End If
	Next 'TempFile
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