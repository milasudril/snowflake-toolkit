#SnowflakeModel Ice crystal Prototype

#Scaling matrices

matrix($global,
    L, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1)

matrix(dip,
    1, 0, 0, h,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1)
    
matrix($global,
    1, 0, 0, 0,
    0, a, 0, 0,
    0, 0, a, 0,
    0, 0, 0, 1)
    
matrix($global,
    -1, 0, 0, 0,
    0, -1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1)
    
mirror_heading()

    

#Mesh subvolumes. Each of these needs to be convex

volume()
	vertex(0.2499998,0.4999992,0.8660259)
	vertex(0.2499994,-0.5000006,0.8660252)
	vertex(-0.2500002,0.4999996,0.8660256)
	vertex(-0.2500006,-0.5000001,0.8660248)
	vertex(-0.25,1.539771e-07,-1.816066e-07)
	vertex(0.25,-7.847913e-08,1.700498e-07)

	face(1,3,0,outside)
	face(2,4,0,inside)
	face(3,1,4,inside)
	face(3,2,0,outside)
	face(4,2,3,outside)
	face(5,1,0,inside)
	face(4,1,5,inside)
	face(4,5,0,outside)

	group(dip,4)
volume()
	vertex(0.2499998,1,6.039064e-07)
	vertex(0.2499994,0.4999998,0.8660256)
	vertex(-0.2500002,0.9999999,1.209014e-07)
	vertex(-0.2500005,0.4999996,0.866025)
	vertex(-0.25,-8.646459e-08,-2.134523e-07)
	vertex(0.25,1.14205e-07,1.422907e-07)

	face(1,3,0,outside)
	face(2,4,0,inside)
	face(3,1,4,inside)
	face(3,2,0,outside)
	face(4,2,3,outside)
	face(5,1,0,inside)
	face(4,1,5,inside)
	face(4,5,0,outside)

	group(dip,4)
volume()
	vertex(0.25,0.5000003,-0.8660251)
	vertex(0.2499996,1,1.031517e-07)
	vertex(-0.25,0.4999999,-0.8660254)
	vertex(-0.2500004,0.9999996,-1.697026e-07)
	vertex(-0.25,-2.026927e-07,-7.583042e-08)
	vertex(0.25,1.549352e-07,1.622574e-08)

	face(1,3,0,outside)
	face(2,4,0,inside)
	face(3,1,4,inside)
	face(3,2,0,outside)
	face(4,2,3,outside)
	face(5,1,0,inside)
	face(4,1,5,inside)
	face(4,5,0,outside)

	group(dip,4)
volume()
	vertex(0.2500001,-0.4999999,-0.8660254)
	vertex(0.2499998,0.4999999,-0.8660255)
	vertex(-0.2499999,-0.5000003,-0.8660253)
	vertex(-0.2500002,0.4999994,-0.8660253)
	vertex(-0.25,-1.380838e-07,8.750734e-08)
	vertex(0.25,6.258585e-08,-7.595042e-08)

	face(1,3,0,outside)
	face(2,4,0,inside)
	face(3,1,4,inside)
	face(3,2,0,outside)
	face(4,2,3,outside)
	face(5,1,0,inside)
	face(4,1,5,inside)
	face(4,5,0,outside)

	group(dip,4)
volume()
	vertex(0.25,-1,4.074439e-08)
	vertex(0.2499999,-0.5000002,-0.8660253)
	vertex(-0.25,-1,2.987727e-07)
	vertex(-0.2500001,-0.5000002,-0.8660249)
	vertex(-0.25,4.275323e-08,1.009639e-07)
	vertex(0.25,-7.049362e-08,-2.980231e-08)

	face(1,3,0,outside)
	face(2,4,0,inside)
	face(3,1,4,inside)
	face(3,2,0,outside)
	face(4,2,3,outside)
	face(5,1,0,inside)
	face(4,1,5,inside)
	face(4,5,0,outside)

	group(dip,4)
volume()
	vertex(0.25,-0.4999999,0.8660254)
	vertex(0.25,-1,2.687177e-07)
	vertex(-0.25,-0.4999997,0.8660256)
	vertex(-0.25,-0.9999996,4.495158e-07)
	vertex(-0.25,9.937668e-08,2.980234e-08)
	vertex(0.25,-5.161913e-08,2.980234e-08)

	face(1,3,0,outside)
	face(2,4,0,inside)
	face(3,1,4,inside)
	face(3,2,0,outside)
	face(4,2,3,outside)
	face(5,1,0,inside)
	face(4,1,5,inside)
	face(4,5,0,outside)

	group(dip,4)
