function corrplot(X,Y,x_label,y_label,dir_base,file_out,X_scale,Y_scale,x_lim,y_lim)
	textwidth=418*0.35136/10;
	figwidth=0.46*textwidth;
	fig=figure('paperunits','centimeters','papersize'...
		,[figwidth 4.5],'paperposition',[0 0 figwidth 4.5]);
	plot(X/X_scale,Y/Y_scale,'.','markersize',1);
	hold on
	if(strcmp(x_label,y_label))
		set(gcf(),'papersize'...
			,[figwidth figwidth],'paperposition',[0 0 figwidth figwidth]);
		xy_min=min(min(X),min(Y));
		xy_min=xy_min-0.1*10^floor(log10(abs(xy_min)));
		xy_max=max(max(X),max(Y));
		xy_max=xy_max+0.1*10^floor(log10(abs(xy_max)));
		x=linspace(xy_min,xy_max,2);
		axis([xy_min xy_max xy_min xy_max]);
		plot(x/X_scale,x/Y_scale,'markersize',1,'--');
		set(gca(),'dataaspectratio', [1, 1, 1]);
	else
		x=linspace(min(X),max(X),2);
		P=polyfit(X,Y,1);
		plot(x/X_scale,polyval(P,x)/Y_scale,'--');
		if(~isempty(x_lim))
			xlim(x_lim);
		end
		ylim(y_lim);
	end

	drawnow
	xlabel(x_label);
	ylabel(y_label);
	hold off

	old_dir=cd(dir_base);
	print(file_out,'-dpdflatex');
	cd(old_dir);

	delete(fig);
end

