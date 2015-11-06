function vr_rel_plot_all()
	pkg load statistics;
	size_pt=0.35136;
	textwidth=418*size_pt/10;
	textheight=595*size_pt/10;
	figwidth=0.9*textwidth;
	figheight=1.2*(textheight-4.75)/3;

	figure('paperunits','centimeters','papersize'...
		,[figwidth figheight],'paperposition',[0 0 figwidth figheight])

	basedir='~/snowflakeout-rates-1';
	dirlist=dir(basedir);
	data=[];
	for k=1:numel(dirlist)
		if(~strcmp(dirlist(k).name,'..') && ~strcmp(dirlist(k).name,'.'))
			basedir2=sprintf('%s/%s',basedir,dirlist(k).name);
			data=[data;vr_rel_plot(1,basedir2,'frame-dropped')];
			data=[data;vr_rel_plot(1,basedir2,'frame')];
		end
	end

	basedir='~/snowflakeout-rates-2';
	dirlist=dir(basedir);
	for k=1:numel(dirlist)
		if(~strcmp(dirlist(k).name,'..') && ~strcmp(dirlist(k).name,'.'))
			basedir2=sprintf('%s/%s',basedir,dirlist(k).name);
			data=[data;vr_rel_plot(1,basedir2,'frame-dropped')];
			data=[data;vr_rel_plot(1,basedir2,'frame')];
		end
	end

	basedir='~/snowflakeout-a-2';
	dirlist=dir(basedir);
	for k=1:numel(dirlist)
		if(~strcmp(dirlist(k).name,'..') && ~strcmp(dirlist(k).name,'.'))
			basedir2=sprintf('%s/%s',basedir,dirlist(k).name);
			data=[data;vr_rel_plot(1,basedir2,'frame-dropped')];
			data=[data;vr_rel_plot(1,basedir2,'frame')];
		end
	end

	basedir='~/snowflakeout-a-1';
	dirlist=dir(basedir);
	for k=1:numel(dirlist)
		if(~strcmp(dirlist(k).name,'..') && ~strcmp(dirlist(k).name,'.'))
			basedir2=sprintf('%s/%s',basedir,dirlist(k).name);
			data=[data;vr_rel_plot(1,basedir2,'frame-dropped')];
			data=[data;vr_rel_plot(1,basedir2,'frame')];
		end
	end


	x=linspace(2e-1,20,512);
	[N,C]=hist3([-data(:,1).*data(:,2),data(:,3)],[256 128]);
	[n,l]=hist(-data(:,1),size(N,2));
	i_nz=find(n>0);
	size(i_nz)
	n=repmat(n,size(N,1),1);
	h=pcolor(C{1}(i_nz),C{2},log10(N(:,i_nz)./n(:,i_nz)));
	set(h,'EdgeColor','none');
	colormap(flipud(gray(256)));
	bar=colorbar;
	ylabel(bar,'Event intensity');
	xlim([0.2,20]);
	set(gca,'xscale','log');
	set(gca,'yscale','log');
	hold on
	plot(x,gamma(4)./(gamma(3).*x));
	xlabel('$-\lambdaR_\text{max}$');
	ylabel('$\frac{v}{v_{r0}}$');
	set(gca,'xtick',[0.2 0.5 1 2 5 10 20]);
	set(gca,'xticklabel',{'0.2','0.5','1','2','5','10','20'});
	hold off
	print('test.png','-dpng','-r600','-G./gswrapper.sh');
end
