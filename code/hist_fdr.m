function [nn,xx]=hist_fdr(x)
%	Use Freedman–Diaconis rule to create a histogram. Otherwise similar to hist.
	n_bins=(size(x,1)^(1/3)) * (max(x(:,1)) - min(x(:,1)))./(2*iqr(x));
	[nn,xx]=hist(x,ceil(n_bins),1);
end
