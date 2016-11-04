function tab=mod34_stats_main_a(do_it)
	a=[0.17,0.22,0.28,0.33,0.39,0.44,0.5,0.55,0.61,0.66];
	row=0;
	for k=1:length(a)
		row=row+1;
		fprintf(2,'Processing %d\n',k);
		tab(row,:)=do_it(a(k));
	end
end
