function tab=mod34_abeta(base_dir)
	a=[0.17,0.22,0.28,0.33,0.39,0.44,0.50,0.55,0.61,0.66];
	tab=zeros(numel(a),5);
	for k=1:numel(a)
		dir=sprintf('%s/%.7g',base_dir,a(k))
		[tau,N,beta_cloud]=beta_comp(dir,'frame');
		i_find=find(beta_cloud);
		[beta_final_c,ratio_c]=limit_estimate(tau(i_find),beta_cloud(i_find));
		fprintf(2,'%.7g %.7g\n',beta_final_c,ratio_c);
		fflush(2);

		[~,~,beta_dropped]=beta_comp(dir,'frame-dropped');
		i_find=find(beta_dropped);
		[beta_final_d,ratio_d]=limit_estimate(tau(i_find),beta_dropped(i_find));
		fprintf(2,'%.7g %.7g\n',beta_final_d,ratio_d);
		fflush(2);

		tab(k,:)=[a(k),beta_final_c,ratio_c,beta_final_d,ratio_d];
	end
end
