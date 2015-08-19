function [tau,N_cloud,beta]=beta_comp(base_dir,prefix)
	frame_data=frame_data_load(base_dir);
	tau=frame_data(:,3);
	N_cloud=frame_data(:,4);
	beta=zeros(size(tau,1),1);
	for k=1:size(frame_data,1)
	% 	R_max	Volume	Speed	L_x	r_xy	r_xz	Subvols
		data=frame_load(base_dir,prefix,frame_data(k,1)/256);
		if size(data,1)>1
			i_g1=find(data(:,7)>1);
			if(size(i_g1,1) > 1)
				fprintf(2,'\rComputing beta for frame %d/%d ',k,size(frame_data,1));
				[alpha,beta_tmp,e_rms]=powerfit(data(:,1),data(:,2));
				beta(k)=beta_tmp;
			end
		end
	end
end
