function do_hist(statfile)
	data=load(statfile);
	[nn,xx]=hist(data(:,1),16);
	figure(1);
	semilogy(xx,nn);
	axis([0 10 1 1000])

	figure(2)
	p=polyfit(log(data(:,1)),log(data(:,2)),1);
	plot(data(:,1),data(:,2),'.');
	hold on
	x=linspace(min(data(:,1)),max(data(:,1)),128);
	plot(x,exp(p(2)).*x.^p(1),'r','linewidth',2);
	hold off
	title(sprintf('%.7g, %.7g',p(1),exp(p(2))));
end
