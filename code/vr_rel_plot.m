function [data]=vr_rel_plot(figno,base_dir,prefix)
	frame_data=frame_data_load(base_dir);
	data=frame_load(base_dir,prefix,frame_data(end,1)/256);
	if size(data,1)>1
		i_g1=find(data(:,7)>1);
		i_1=find(data(:,7)==1);
		if(size(i_g1,1) > 1)
		%	fprintf(2,'\rComputing v_r for frame %d/%d ',k,size(frame_data,1));

			[~,beta,e_rms]=powerfit(data(i_g1,1),data(i_g1,2));

			[N,R] = hist_fdr(data(i_g1,1));
			[~,i_max]=max(N);
			n_0=sum(N(i_max:end));
			cum=1-cumsum(N(i_max:end))/n_0;
			[~,lambda,e_rms]=expfit(R(i_max:end)',cum');

			vr0=3*mean(data(i_g1,2))./(4*pi*mean(data(i_g1,1).^3));
			vr=3*data(i_g1,2)./(4*pi*data(i_g1,1).^3);

		%	figure(figno);
			data=[lambda.*ones(numel(i_g1),1),data(i_g1,1),vr./vr0];
		%	hold on
		%	loglog(,'.','markersize',0.25);
		end
	end
end
