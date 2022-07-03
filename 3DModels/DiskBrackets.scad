module vertical() {
	difference() {
		union() {
		translate([0,12,0])
			cube([10,3,10]);
		translate([0,7,7])
			fillet(10,10);
		translate([10/2,15,10])
			rotate([90,0,0])
				cylinder(3,d=10,$fn=100);		
		}
		translate([(10/2),16,10])
			rotate([90,0,0])
				cylinder(10,d=3.5,$fn=100);
		translate([(10/2),13.5,10])
			rotate([90,0,0])
				cylinder(10,d=7,$fn=100);		
	}
}

module fillet(filletsize,width)
{
	difference() {
	translate([0,-filletsize/2,-filletsize/2])
		cube([width,filletsize,filletsize]);
	translate([-1,0,0])
		rotate([0,90,0])
			cylinder(width+2,d=filletsize,$fn=100);
    translate([-1,-filletsize,-filletsize/2])
		cube([width+2,filletsize,filletsize]);
    translate([-1,-filletsize/2,0])
		cube([width+2,filletsize,filletsize]);		
	}
}


module tab() {
difference() {
	union() {
		translate([0,4,0])
        cube([10,6,2]);
		translate([10/2,4,0])
			cylinder(2,d=10,$fn=100);
		}
    translate([10/2,4,-1])			
		countersink(3);
	}
}

module countersink(drillsize)
	union() {
		cylinder(drillsize*3,0,drillsize*3,$fn=100);
		translate([0,0,-5])
		cylinder(10,d=drillsize,$fn=100);
	}
	
module bracket() {
	union() {
			tab();
			vertical();
			translate([0,132,0])
				mirror([0,1,0]) {
					union() {
						tab();
						vertical();
					}
			}
			translate([0,10,0])
				cube([10,112,2]);
		}
}


bracket();