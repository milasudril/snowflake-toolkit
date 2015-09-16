function [tau,N_cloud,vr]=vr_comp(base_dir,prefix)
	frame_data=frame_data_load(base_dir);
	tau=frame_data(:,3);
	N_cloud=frame_data(:,4);
	vr=zeros(size(tau,1),1);
	for k=1:size(frame_data,1)
	% 	R_max	Volume	Speed	L_x	r_xy	r_xz	Subvols
		data=frame_load(base_dir,prefix,frame_data(k,1)/256);
		if size(data,1)>1
			i_g1=find(data(:,7)>1);
			if(size(i_g1,1) > 1)
				fprintf(2,'\rComputing v_r for frame %d/%d ',k,size(frame_data,1));
				vr(k)=3*mean(data(i_g1,2))./(4*pi*mean(data(i_g1,1).^3));
			end
		end
	end
end
