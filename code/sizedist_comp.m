function [tau,N_cloud,lambda]=sizedist_comp(base_dir,prefix)
	frame_data=frame_data_load(base_dir);
	tau=frame_data(:,3);
	N_cloud=frame_data(:,4);
	lambda=zeros(size(tau,1),1);
	for k=1:size(frame_data,1)
	% 	R_max	Volume	Speed	L_x	r_xy	r_xz	Subvols
		data=frame_load(base_dir,prefix,frame_data(k,1)/256);
		if size(data,1)>1
			fprintf(2,'\rComputing histogram for frame %d/%d ',k,size(frame_data,1));
			i_1=find(data(:,7)==1);
			r_0=mean(data(i_1,1));
			i_g1=find(data(:,7)>1);
			if size(i_g1,1)>1
				[N,R] = hist_fdr(data(i_g1,1)/r_0);
				[~,i_max]=max(N);
				n_0=sum(N(i_max:end));
				cum=1-cumsum(N(i_max:end))/n_0;
				[~,lambda_tmp,e_rms]=expfit(R(i_max:end)',cum');
				lambda(k)=lambda_tmp;
			end
		end
		data=[];
	end
end
