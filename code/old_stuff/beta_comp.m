function [tau,N_cloud,beta,alpha]=beta_comp(base_dir,prefix)
	[frame_data,N_frames]=frame_data_load(base_dir);
	frame=frame_data.('# Frame');
	tau=frame_data.('Simulation time');
	N_cloud=frame_data.N_cloud;
	beta=zeros(size(tau,1),1);
	alpha=beta;
	for k=1:N_frames
	% 	R_max	Volume	Speed	L_x	r_xy	r_xz	Subvols
		[data,N_rows]=frame_load(base_dir,prefix,frame(k));
		if N_rows>1
			N_v=data.('Number of sub-volumes');
			i_g1=find(N_v>1);
			i_1=find(N_v==1);
			R_max=data.('# R_max');
			r_0=mean(R_max(i_1));
			if(size(i_g1,1) > 1)
				fprintf(2,'\rComputing beta for frame %d/%d ',k,N_frames);
				[kappa,beta_tmp,e_rms]=powerfit(R_max(i_g1),data.Volume(i_g1));
				beta(k)=beta_tmp;
				alpha(k)=kappa*r_0.^(beta_tmp-3);
			end
		end
	end
end
