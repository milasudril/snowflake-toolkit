function [tau,N_cloud,r_xy,r_xz]=mod34_aspect_ratio_comp(base_dir,growthrate...
	,meltrate,droprate,prefix)
	frame_data=frame_data_load(base_dir,growthrate,meltrate,droprate);
	tau=frame_data(:,3);
	N_cloud=frame_data(:,4);
	r_xy=zeros(size(tau,1),1);
	r_xz=zeros(size(tau,1),1);
	for k=1:size(frame_data,1)
	% 	R_max	Volume	Speed	L_x	r_xy	r_xz
		data=frame_load(base_dir,growthrate,meltrate,droprate,prefix...
			,frame_data(k,1));
		if size(data,1)>1
			fprintf(2,'\rComputing ratio for frame %d/%d ',k,size(frame_data,1));
			r_xy(k)=mean(data(:,5));
			r_xz(k)=mean(data(:,6));
		end
	end
end
