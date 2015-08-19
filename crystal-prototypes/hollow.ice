#SnowflakeModel Ice crystal Prototype

#Scaling matrices

matrix(dip_l,
    1, 0, 0, h,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1)

matrix(dip_r,
    1, 0, 0, mult(h,-1),
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1)
  
matrix(end_l,
    1,0,0,mult(L,-0.5),
    0,1,0,0,
    0,0,1,0,
    0,0,0,1)
    
matrix(end_r,
    1,0,0,mult(L,0.5),
    0,1,0,0,
    0,0,1,0,
    0,0,0,1)
    
mirror_heading()

    

#Mesh subvolumes. Each of these needs to be convex

volume()
	vertex(-0.9999999,0.4999999,0.8660254)
	vertex(-1,1,2.687177e-07)
	vertex(-2,0.5,0.8660256)
	vertex(-2,1,4.495158e-07)
	vertex(-1,5.161913e-08,2.980234e-08)
	vertex(0,1,2.687177e-07)
	vertex(0,5.161913e-08,2.980234e-08)
	vertex(1.192093e-07,0.4999999,0.8660254)

	face(1,3,0,outside)
	face(2,4,0,inside)
	face(3,1,4,inside)
	face(3,2,0,outside)
	face(4,6,7,inside)
	face(4,2,3,outside)
	face(6,5,7,inside)
	face(0,7,5,outside)
	face(1,5,6,inside)
	face(0,4,7,inside)
	face(1,0,5,outside)
	face(4,1,6,inside)

	group(dip_r,4)
	group(end_r,2,3)
volume()
	vertex(-0.9999999,-0.5000001,0.8660253)
	vertex(-0.9999999,0.4999998,0.8660255)
	vertex(-2,-0.5000001,0.8660256)
	vertex(-2,0.4999998,0.8660257)
	vertex(-1,-1.080602e-14,5.960464e-08)
	vertex(1.192093e-07,0.4999998,0.8660255)
	vertex(0,-1.080602e-14,5.960464e-08)
	vertex(1.192093e-07,-0.5000001,0.8660253)

	face(1,3,0,outside)
	face(2,4,0,inside)
	face(3,1,4,inside)
	face(3,2,0,outside)
	face(4,6,7,inside)
	face(4,2,3,outside)
	face(6,5,7,inside)
	face(0,7,5,outside)
	face(1,5,6,inside)
	face(0,4,7,inside)
	face(1,0,5,outside)
	face(4,1,6,inside)

	group(dip_r,4)
	group(end_r,2,3)
volume()
	vertex(-1,-1,-8.990375e-08)
	vertex(-0.9999999,-0.5000002,0.8660252)
	vertex(-2,-1,9.089437e-08)
	vertex(-2,-0.5000003,0.8660254)
	vertex(-1,-5.161915e-08,2.980231e-08)
	vertex(1.192093e-07,-0.5000002,0.8660252)
	vertex(0,-5.161915e-08,2.980231e-08)
	vertex(0,-1,-8.990375e-08)

	face(1,3,0,outside)
	face(2,4,0,inside)
	face(3,1,4,inside)
	face(3,2,0,outside)
	face(4,2,3,outside)
	face(6,5,7,inside)
	face(4,6,7,inside)
	face(1,0,7,outside)
	face(1,5,6,inside)
	face(0,4,7,inside)
	face(5,1,7,outside)
	face(4,1,6,inside)

	group(dip_r,4)
	group(end_r,2,3)
volume()
	vertex(-1,-0.4999999,-0.8660255)
	vertex(-1,-1,-3.283223e-07)
	vertex(-2,-0.4999999,-0.8660253)
	vertex(-2,-1,-2.071288e-07)
	vertex(-1,-5.161913e-08,-2.980234e-08)
	vertex(-1.192093e-07,-0.4999999,-0.8660255)
	vertex(0,-1,-3.283223e-07)
	vertex(0,-5.161913e-08,-2.980234e-08)

	face(1,3,0,outside)
	face(2,4,0,inside)
	face(3,1,4,inside)
	face(3,2,0,outside)
	face(4,7,5,inside)
	face(4,2,3,outside)
	face(7,6,5,inside)
	face(1,0,5,outside)
	face(1,6,7,inside)
	face(0,4,5,inside)
	face(6,1,5,outside)
	face(4,1,7,inside)

	group(dip_r,4)
	group(end_r,2,3)
volume()
	vertex(-1,0.5000001,-0.8660253)
	vertex(-1,-0.4999997,-0.8660256)
	vertex(-2,0.5000001,-0.8660253)
	vertex(-2,-0.4999997,-0.8660255)
	vertex(-1,1.601682e-14,-5.960464e-08)
	vertex(-1.192093e-07,-0.4999997,-0.8660256)
	vertex(0,1.601682e-14,-5.960464e-08)
	vertex(-1.192093e-07,0.5000001,-0.8660253)

	face(1,3,0,outside)
	face(2,4,0,inside)
	face(3,1,4,inside)
	face(3,2,0,outside)
	face(4,6,7,inside)
	face(4,2,3,outside)
	face(6,5,7,inside)
	face(0,7,5,outside)
	face(1,5,6,inside)
	face(0,4,7,inside)
	face(1,0,5,outside)
	face(4,1,6,inside)

	group(dip_r,4)
	group(end_r,2,3)
volume()
	vertex(-1,1,2.687177e-07)
	vertex(-1,0.5000004,-0.8660251)
	vertex(-2,1,3.899112e-07)
	vertex(-2,0.5000005,-0.8660251)
	vertex(-1,5.161915e-08,-2.980231e-08)
	vertex(-1.192093e-07,0.5000004,-0.8660251)
	vertex(0,5.161915e-08,-2.980231e-08)
	vertex(0,1,2.687177e-07)

	face(1,3,0,outside)
	face(2,4,0,inside)
	face(3,1,4,inside)
	face(3,2,0,outside)
	face(4,2,3,outside)
	face(6,5,7,inside)
	face(4,6,7,inside)
	face(0,7,5,outside)
	face(1,5,6,inside)
	face(0,4,7,inside)
	face(1,0,5,outside)
	face(4,1,6,inside)

	group(dip_r,4)
	group(end_r,2,3)
