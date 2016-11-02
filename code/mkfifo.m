function name=mkfifo()
% hack to generate a fifo with unique name
	rng=fopen('/dev/urandom','r');
	x=fread(rng,1,'uint32');
	y=fread(rng,3,'uint16');
	z=fread(rng,3,'uint16');
	fclose(rng);
	name=sprintf('/tmp/%08x-%04x-%04x-%04x-%04x%04x%04x',x,y(1),y(2),y(3),z(1),z(2),z(3));
	system_wrapper({'mkfifo',name},1);
end