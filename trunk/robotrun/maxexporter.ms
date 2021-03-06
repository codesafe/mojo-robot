function exportData filehandle obj fixvalue neg tempcontroller = 
(
	local prevtime = 0
    local count  = tempcontroller.keys.count
	
	if count >= 1 then (
		format "\t\t<part deviceid=\"%\">\n" obj.name to:filehandle	
	)
	
     for i = 1 to count do (     
		local t = tempcontroller.keys[i].time / 30
		local r = tempcontroller.keys[i].value   
		r = radToDeg(r) * neg + fixvalue
		-- r = r * neg + degToRad(fixvalue)
		 
		 if t > 0 then (
			format "\t\t\t<motion>\n" to:filehandle
			format "\t\t\t\t<starttime>%</starttime>\n" prevtime to:filehandle
			format "\t\t\t\t<endtime>%</endtime>\n" t to:filehandle
			format "\t\t\t\t<angle>%</angle>\n" r to:filehandle
			format "\t\t\t</motion>\n" to:filehandle
		 )
		prevtime = t;		 
     )
	
	if count >= 1 then (
		format "\t\t</part>\n" to:filehandle	
	)	 
)

-- note keys is must be in body

function exportNote filehandle obj =
(
	local prevtime = 0
	ct = numNoteTracks obj
	if ct > 0 then (

		nt = getNoteTrack obj 1
		local count = nt.keys.count
		
		if count >= 1 then (
			format "\t\t<part deviceid=\"%\">\n" "display" to:filehandle	
		)
		
		for i = 1 to count do (     
			keys = nt.keys[i]
			local t = keys.time / 30
			local v = keys.value

			if t > 0 then (
				format "\t\t\t<motion>\n" to:filehandle
				format "\t\t\t\t<starttime>%</starttime>\n" prevtime to:filehandle
				format "\t\t\t\t<endtime>%</endtime>\n" t to:filehandle
				format "\t\t\t\t<note>%</note>\n" v to:filehandle
				format "\t\t\t</motion>\n" to:filehandle
			 )
			 prevtime = t;
		)

		if count >= 1 then (
			format "\t\t</part>\n" to:filehandle	
		)	 	
		
	)

)

function startXML filehandle aniname =
(
	format "<data>\n" to:filehandle
	format "\t<animation name=\"%\">\n" aniname to:filehandle
)

function endXML filehandle =
(
	format "\t</animation>\n" to:filehandle
	format "</data>\n" to:filehandle
)

function RotationX filehandle obj fixvalue neg =
(
	tempcontroller = obj.rotation.controller.X_rotation.controller.GetLimitedControl()
	exportData filehandle obj fixvalue neg tempcontroller
)

function RotationY filehandle obj fixvalue neg =
(
	tempcontroller = obj.rotation.controller.Y_rotation.controller.GetLimitedControl()
	exportData filehandle obj fixvalue neg tempcontroller
)

function RotationZ filehandle obj fixvalue neg =
(
	tempcontroller = obj.rotation.controller.Z_rotation.controller.GetLimitedControl()
	exportData filehandle obj fixvalue neg tempcontroller	
)

rollout ExportAnimation "Export Animation" width:500 height:228
(
	Global saveFilename = ""

	label lbl2 "Enter Animation name" pos:[10,30] width:500 height:20
	editText aniname "" pos:[120,30] align:#center width:380  height:30
	Button SetOutFile "Set output Filename" pos:[0,80] align:#center width:500 height:30
	Button ExportAnim "Export Animation" pos:[0,120]  align:#center width:500 height:30

	on setoutfile pressed do
	(
		Global OutFile = getSaveFileName caption:"Choose the output filename"
		
		if OutFile != undefined then (
			SetOutFile.text = OutFile
			saveFilename = OutFile
		)
			
	)
	 
	on ExportAnim pressed do
	(
		if aniname.text == "" then (
			messageBox "Please enter Animation name!!"
		)
		else (
			if saveFilename == "" then (
				messageBox "Please choose output path!!"
			)
			else (
				MyOutFile = OutFile
				AnimFile = CreateFile MyOutFile
				_name = aniname.text			

				startXML AnimFile _name
				
				RotationY AnimFile $41 180 1
				RotationX AnimFile $42 180 -1

				RotationY AnimFile $44 180 1
				RotationX AnimFile $43 180 1

				RotationZ AnimFile $51 180 1
				RotationX AnimFile $52 180 1
				RotationY AnimFile $53 180 1
				RotationX AnimFile $54 180 1

				-- Note Tracks for Display
				exportNote AnimFile $body 
				
				endXML AnimFile

				Close AnimFile		
				messageBox "Export Done !!"			
			)
		)
	)
)
 
CreateDialog ExportAnimation
