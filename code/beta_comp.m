function [tau,N_cloud,beta,alpha]=beta_comp(base_dir,prefix)
	frame_data=frame_data_load(base_dir);
	tau=frame_data(:,3);
	N_cloud=frame_data(:,4);
	beta=zeros(size(tau,1),1);
	alpha=beta;
	for k=1:size(frame_data,1)
	% 	R_max	Volume	Speed	L_x	r_xy	r_xz	Subvols
		data=frame_load(base_dir,prefix,frame_data(k,1));
		if size(data,1)>1
			i_g1=find(data(:,7)>1);
			i_1=find(data(:,7)==1);
			r_0=mean(data(i_1,1));
			if(size(i_g1,1) > 1)
				fprintf(2,'\rComputing beta for frame %d/%d ',k,size(frame_data,1));
				[kappa,beta_tmp,e_rms]=powerfit(data(i_g1,1),data(i_g1,2));
				beta(k)=beta_tmp;
				alpha(k)=kappa*r_0.^(beta_tmp-3);
			end
		end
	end
end
