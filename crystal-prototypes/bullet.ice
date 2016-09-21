#SnowflakeModel Ice crystal Prototype

#Scaling matrices

#front is the tip vertex

matrix(front,
	t,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1)


#back is the flat end

matrix(back,
	L,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1)


matrix($scale_a,
	1,0,0,0,
	0,a,0,0,
	0,0,a,0,
	0,0,0,1)

matrix($scale_tot,
	s,0,0,0,
	0,s,0,0,
	0,0,s,0,
	0,0,0,1)

#Mesh subvolumes. Each of these needs to be convex

volume()
	vertex(-1,0,-5.960464e-08)
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

	face(0,2,4,outside)
	face(1,5,3,outside)
	face(3,5,4,outside)
	face(0,4,6,outside)
	face(1,7,5,outside)
	face(5,7,6,outside)
	face(0,6,8,outside)
	face(1,9,7,outside)
	face(7,9,8,outside)
	face(0,8,10,outside)
	face(1,11,9,outside)
	face(9,11,10,outside)
	face(0,10,12,outside)
	face(1,13,11,outside)
	face(11,13,12,outside)
	face(0,12,2,outside)
	face(1,3,13,outside)
	face(13,3,2,outside)
	face(2,3,4,outside)
	face(4,5,6,outside)
	face(6,7,8,outside)
	face(8,9,10,outside)
	face(10,11,12,outside)
	face(12,13,2,outside)

	group(back,0,2,4,6,8,10,12)
	group(front,1)
