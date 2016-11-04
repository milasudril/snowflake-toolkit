function sizedistplot_temp(base_dir,prefix,frame,c)
	data=frame_load(base_dir,prefix,frame);
	
		N_v=data.('Number of sub-volumes');
		R_max=data.('# R_max');
		i_1=find(N_v==1);
		r_0=mean(R_max(i_1,1));
		i_g1=find(N_v>1);
		if size(i_g1,1)>1
			[N,R] = hist_fdr(R_max(i_g1)/r_0);
			[~,i_max]=max(N);
			semilogy(R,N,c);
			hold on
		end
end