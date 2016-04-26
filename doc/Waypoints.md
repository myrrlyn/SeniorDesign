# Coordinates

These are the list of GPS coordinates along the route we are running between the
University Center and Fawick Hall.

Coordinates are formatted as follows

`DD° MM mmmm H`

where DD is the degree, MM is the minute, and mmmm is the fractional minute. H
is the hemisphere.

1. **University Center Terminus**

    This is on the large block of sidewalk at the UC east entrance, by the north
    handicap control button.

    The robot MUST face down the sidewalk for the run to begin cleanly.

    41° 37 9700 N
    85° 00 4493 W

2. **UC Sidewalk Choke**

    This is the junction between the main run of the sidewalk and where the UC
    landing widens. The robot should hit this on the center line and continue to
    the railroad tracks.

    41° 37 9700 N
    85° 00 4350 W

3. **Railroad Tracks**

    This waypoint is in the center of the railroad tracks. Obviously, if the
    robot stops here, that is a less than ideal situation and should be
    corrected as soon as possible.

    41° 37 9675 N
    85° 00 4287 W

4. **West Bock Intersection**

    Sidewalks from Bock intersect the main sidewalk thrice. This is the western
    of the three intersections.

    41° 37 9631 N
    85° 00 4181 W

5. **West Best Intersection**

    Best Hall also has two intersections with the main sidewalk. This is the
    west.

    41° 37 9632 N
    85° 00 4037 W

6. **Central Bock Intersection**

    41° 37 9633 N
    85° 00 3880 W

7. **Lamppost**

    This is the only lamppost on the south side of the main sidewalk.

    41° 37 9634 N
    85° 00 3918 W

8. **East South Shoulder**

    There are two concrete shoulders on the sidewalk. We do not navigate into
    them, but they are useful landmarks. This is the eastern.

    41° 37 9635 N
    85° 00 3806 W

9. **East Best Hall Intersection**

    This is the eastern of the Best Hall sidewalk intersections.

    41° 37 9636 N
    85° 00 3762 W

10. **Centennial Court Choke**

    This is the junction between the main run of the sidewalk and the Centennial
    courtyard.

    41° 37 9637 N
    85° 00 3706 W

11. **East Bock Intersection**

    This is the east-most spur sidewalk from Bock Hall.

    41° 37 9610 N
    85° 00 3662 W

12. **SouthWest Corner of Courtyard**

    This is the southwest corner of the Centennial courtyard.

    41° 37 9590 N
    85° 00 3590 W

13. **Firepit**

    This waypoint is on the south edge of the courtyard directly south of the
    fire ring.

    41° 37 9595 N
    85° 00 3512 W

14. **Fawick Hall Terminus**

    We are not taking the Fawick sidewalk south to park directly at its door,
    since that requires fine control in a GPS shadow and is something we do not
    have the technical capacity to achieve. Instead, the route ends north of and
    parallel to Fawick's east face.

    41° 37 9600 N
    85° 00 3425 W

# Route

The waypoints are indexed in route order. For the UC → Fawick leg, start at the
UC and count up. For the Fawick → UC leg, start at Fawick and count down.

The offset between the target coordinate and current location provides the
range and bearing to travel.

A single trip is defiend as UC → Fawick → UC. The robot will pause at Fawick and
await instructions, and halt at UC. It will pivot at Fawick, but must be
manually reset at the UC terminus.
