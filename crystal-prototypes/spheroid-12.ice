#SnowflakeModel Ice crystal Prototype

#Scaling matrices

matrix($size,
    r_x,   0,  0,0,
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
	vertex(0,0,-1)
	vertex(0.7236,-0.52572,-0.447215)
	vertex(-0.276385,-0.85064,-0.447215)
	vertex(-0.894425,0,-0.447215)
	vertex(-0.276385,0.85064,-0.447215)
	vertex(0.7236,0.52572,-0.447215)
	vertex(0.276385,-0.85064,0.447215)
	vertex(-0.7236,-0.52572,0.447215)
	vertex(-0.7236,0.52572,0.447215)
	vertex(0.276385,0.85064,0.447215)
	vertex(0.894425,0,0.447215)
	vertex(0,0,1)

	face(0,1,2,outside)
	face(1,0,5,outside)
	face(0,2,3,outside)
	face(0,3,4,outside)
	face(0,4,5,outside)
	face(1,5,10,outside)
	face(2,1,6,outside)
	face(3,2,7,outside)
	face(4,3,8,outside)
	face(5,4,9,outside)
	face(1,10,6,outside)
	face(2,6,7,outside)
	face(3,7,8,outside)
	face(4,8,9,outside)
	face(5,9,10,outside)
	face(6,10,11,outside)
	face(7,6,11,outside)
	face(8,7,11,outside)
	face(9,8,11,outside)
	face(10,9,11,outside)

