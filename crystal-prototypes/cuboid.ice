#SnowflakeModel Ice crystal Prototype

#Scaling matrices

matrix($size,
    r_x,  0,  0,0,
      0,r_y,  0,0,
      0,  0,r_z,0,
      0,  0,  0,1)


matrix($scale_tot,
	s,0,0,0,
	0,s,0,0,
	0,0,s,0,
	0,0,0,1) 

#Mesh subvolumes. Each of these needs to be convex

volume()
	vertex(1,0.9999999,-1)
	vertex(1,-1,-1)
	vertex(-1,-0.9999998,-1)
	vertex(-0.9999996,1,-1)
	vertex(1,0.9999995,1)
	vertex(0.9999993,-1.000001,1)
	vertex(-1,-0.9999996,1)
	vertex(-0.9999999,1,1)

	face(1,2,3,outside)
	face(7,6,5,outside)
	face(4,5,1,outside)
	face(5,6,2,outside)
	face(2,6,7,outside)
	face(0,3,7,outside)
	face(0,1,3,outside)
	face(4,7,5,outside)
	face(0,4,1,outside)
	face(1,5,2,outside)
	face(3,2,7,outside)
	face(4,0,7,outside)

