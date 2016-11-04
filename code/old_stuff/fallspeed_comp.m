function [tau,N_cloud,fallspeed]=beta_comp(base_dir,growthrate...
	,meltrate,droprate,prefix)
	frame_data=frame_data_load(base_dir,growthrate,meltrate,droprate);
	tau=frame_data(:,3);
	N_cloud=frame_data(:,4);
	fallspeed=zeros(size(tau,1),1);
	for k=1:size(frame_data,1)
	% 	R_max	Volume	Speed	L_x	r_xy	r_xz
		data=frame_load(base_dir,growthrate,meltrate,droprate,prefix...
			,frame_data(k,1));
		if size(data,1)>1
			fprintf(2,'\rComputing speed for frame %d/%d ',k,size(frame_data,1));
			fallspeed(k)=mean(data(:,3));
		end
	end
end
