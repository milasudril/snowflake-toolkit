#SnowflakeModel Ice crystal Prototype

#Scaling matrices

matrix(tip,
    t, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1)

matrix(front,
    R, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1)
    
mirror_heading()

#global scaling matrix

matrix($global,
	1,0,0,0,
	0,a,0,0,
	0,0,a,0,
	0,0,0,1)


#Mesh subvolumes. Each of these needs to be convex

volume()
	vertex(1,0,5.960464e-08)
	vertex(-1,1,-5.960464e-08)
	vertex(0,1,5.960464e-08)
	vertex(-1,0.5,-0.8660255)
	vertex(1.192093e-07,0.5,-0.8660254)
	vertex(-1,-0.5000001,-0.8660254)
	vertex(1.192093e-07,-0.5000001,-0.8660253)
	vertex(-1,-1,0)
	vertex(0,-1,1.788139e-07)
	vertex(-1,-0.4999999,0.8660254)
	vertex(0,-0.4999999,0.8660255)
	vertex(-1,0.4999999,0.8660254)
	vertex(0,0.4999999,0.8660255)
	vertex(-1,-7.450581e-08,0)

	face(0,4,2,outside)
	face(2,4,3,outside)
	face(0,6,4,outside)
	face(4,6,5,outside)
	face(0,8,6,outside)
	face(6,8,7,outside)
	face(0,10,8,outside)
	face(8,10,9,outside)
	face(0,12,10,outside)
	face(10,12,11,outside)
	face(0,2,12,outside)
	face(12,2,1,outside)
	face(1,2,3,outside)
	face(3,4,5,outside)
	face(5,6,7,outside)
	face(7,8,9,outside)
	face(9,10,11,outside)
	face(11,12,1,outside)
	face(5,13,3,inside)
	face(7,13,5,inside)
	face(9,13,7,inside)
	face(11,13,9,inside)
	face(11,1,13,inside)
	face(3,13,1,inside)

	group(tip,0)
	group(back,1,3,5,7,9,11,13)
