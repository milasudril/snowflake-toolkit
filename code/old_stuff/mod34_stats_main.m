function tab=mod34_stats_main(do_it)
	growthrate=[15000000,22000000,33000000];
	meltrate=[500,1000,2000];
	droprate=[10000,20000,50000];
	row=0;
	for k=1:length(growthrate)
		for l=1:length(meltrate)
			for m=1:length(droprate)
				row=row+1;
				fprintf(2,'Processing %d %d %d\n',k,l,m);
				tab(row,:)=do_it(growthrate(k),meltrate(l),droprate(m));
			end
		end
	end
end

