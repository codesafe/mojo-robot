/*
	Max Exporter 
*/

function exportData obj fixvalue neg tempcontroller = 
(
	local prevtime = 0
    local count  = tempcontroller.keys.count
	
	if count > 1 then
		format "<part deviceid=\"%\">\n" obj.name	
	
     for i = 1 to count do (     
		local t = tempcontroller.keys[i].time / 30
		local r = tempcontroller.keys[i].value   
		r = radToDeg(r) * neg + fixvalue
		 
		 if t > 0 then (
			format "\t<motion>\n"
			format "\t\t<starttime>%</starttime>\n" prevtime
			format "\t\t<endtime>%</endtime>\n" t
			format "\t\t<angle>%</angle>\n" r
			format "\t</motion>\n"
		 )
		prevtime = t;		 
     )
	
	if count > 1 then	 
		format "</part>\n"
)

function RotationX obj fixvalue neg =
(
	tempcontroller = obj.rotation.controller.X_rotation.controller.GetLimitedControl()
	exportData obj fixvalue neg tempcontroller
)

function RotationY obj fixvalue neg =
(
	tempcontroller = obj.rotation.controller.Y_rotation.controller.GetLimitedControl()
	exportData obj fixvalue neg tempcontroller
)

function RotationZ obj fixvalue neg =
(
	tempcontroller = obj.rotation.controller.Z_rotation.controller.GetLimitedControl()
	exportData obj fixvalue neg tempcontroller	
)

function Export =
(
	RotationY $41 180 1
	RotationX $42 180 -1

	RotationY $44 180 1
	RotationX $43 180 1

	RotationZ $51 180 1
	RotationX $52 180 1
	RotationY $53 180 1
	RotationX $54 180 1
)


Export() 