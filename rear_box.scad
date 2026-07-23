// ==========================================
// HIDDEN HEADPHONES - REAR ELECTRONICS BOX (V2)
// Designed for 3D Printing (PETG / TPU)
// ==========================================

$fn = 60; // Smooth curves

// --- Dimensions ---
inner_length = 55;
inner_width  = 30;
inner_height = 18;
wall_thick   = 2.0;

box_length = inner_length + (wall_thick * 2);
box_width  = inner_width + (wall_thick * 2);
box_height = inner_height + (wall_thick * 2);
corner_r   = 4.0;

// --- Main Assembly ---
// Uncomment one at a time to export individual STL parts:
main_bottom_case();
// translate([0, 0, 30]) main_top_lid();

// --- Modules ---

module rounded_cube(x, y, z, r) {
    hull() {
        translate([r, r, r]) sphere(r=r);
        translate([x-r, r, r]) sphere(r=r);
        translate([r, y-r, r]) sphere(r=r);
        translate([x-r, y-r, r]) sphere(r=r);
        translate([r, r, z-r]) sphere(r=r);
        translate([x-r, r, z-r]) sphere(r=r);
        translate([r, y-r, z-r]) sphere(r=r);
        translate([x-r, y-r, z-r]) sphere(r=r);
    }
}

module main_bottom_case() {
    difference() {
        // Outer Body
        rounded_cube(box_length, box_width, box_height * 0.7, corner_r);
        
        // Inner Cavity
        translate([wall_thick, wall_thick, wall_thick])
            cube([inner_length, inner_width, inner_height + 5]);
        
        // USB-C Charging Port Cutout (Bottom / Back Edge)
        translate([box_length/2 - 6, -1, wall_thick + 2])
            cube([12, wall_thick + 2, 6]);

        // Conduit Openings (Left and Right for Neckband Wires)
        translate([-1, box_width/2, box_height/2 - 2])
            rotate([0, 90, 0]) cylinder(h=wall_thick + 2, r=3.5);
            
        translate([box_length - wall_thick - 1, box_width/2, box_height/2 - 2])
            rotate([0, 90, 0]) cylinder(h=wall_thick + 2, r=3.5);

        // M2 Screw Heat-Set Insert Holes (Corners)
        screw_holes(h=box_height, r=1.6);
    }
    
    // Internal Mounting Posts for Stacking PCBs
    translate([wall_thick + 4, wall_thick + 4, wall_thick]) cylinder(h=3, r=2.5);
    translate([box_length - wall_thick - 4, wall_thick + 4, wall_thick]) cylinder(h=3, r=2.5);
    translate([wall_thick + 4, box_width - wall_thick - 4, wall_thick]) cylinder(h=3, r=2.5);
    translate([box_length - wall_thick - 4, box_width - wall_thick - 4, wall_thick]) cylinder(h=3, r=2.5);
}

module main_top_lid() {
    difference() {
        // Lid Shell
        rounded_cube(box_length, box_width, box_height * 0.35, corner_r);
        
        // Inner Lip Recess
        translate([wall_thick, wall_thick, -1])
            cube([inner_length, inner_width, box_height * 0.35 - wall_thick + 1]);

        // M2 Screw Countersunk Clearance Holes
        screw_holes(h=box_height, r=1.2);
    }
}

module screw_holes(h, r) {
    translate([wall_thick + 3, wall_thick + 3, -1]) cylinder(h=h, r=r);
    translate([box_length - wall_thick - 3, wall_thick + 3, -1]) cylinder(h=h, r=r);
    translate([wall_thick + 3, box_width - wall_thick - 3, -1]) cylinder(h=h, r=r);
    translate([box_length - wall_thick - 3, box_width - wall_thick - 3, -1]) cylinder(h=h, r=r);
}