function animate(basepath,N)
	for k=N(1):N(2)
		filename=sprintf('%s-%d.txt',basepath,k);
		do_hist(filename);
		drawnow;
		pause(0.05);
	end
end