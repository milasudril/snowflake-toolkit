[tau,N_cloud,beta,alpha]=beta_comp('~/snowflakeout-rates-1/44000000-2000-500','frame');
i_nz=find(beta>0);
limit_estimate(tau(i_nz),alpha(i_nz))
i_nz=find(alpha>0);
limit_estimate(tau(i_nz),beta(i_nz))

[tau,N_cloud,beta,alpha]=beta_comp('~/snowflakeout-rates-1/44000000-2000-500','frame-dropped');
i_nz=find(beta>0);
limit_estimate(tau(i_nz),alpha(i_nz))
i_nz=find(alpha>0);
limit_estimate(tau(i_nz),beta(i_nz))

[tau,N_cloud,gamma]=sizedist_comp('~/snowflakeout-rates-1/44000000-2000-500','frame');
i_nz=find(gamma<0);
limit_estimate(tau(i_nz),gamma(i_nz))

[tau,N_cloud,gamma]=sizedist_comp('~/snowflakeout-rates-1/44000000-2000-500','frame-dropped');
i_nz=find(gamma<0);
limit_estimate(tau(i_nz),gamma(i_nz))



[tau,N_cloud,beta,alpha]=beta_comp('~/snowflakeout-other/44000000-2000-500','frame');
i_nz=find(beta>0);
limit_estimate(tau(i_nz),alpha(i_nz))
i_nz=find(alpha>0);
limit_estimate(tau(i_nz),beta(i_nz))

[tau,N_cloud,beta,alpha]=beta_comp('~/snowflakeout-other/44000000-2000-500','frame-dropped');
i_nz=find(beta>0);
limit_estimate(tau(i_nz),alpha(i_nz))
i_nz=find(alpha>0);
limit_estimate(tau(i_nz),beta(i_nz))

[tau,N_cloud,gamma]=sizedist_comp('~/snowflakeout-other/44000000-2000-500','frame');
i_nz=find(gamma<0);
limit_estimate(tau(i_nz),gamma(i_nz))

[tau,N_cloud,gamma]=sizedist_comp('~/snowflakeout-other/44000000-2000-500','frame-dropped');
i_nz=find(gamma<0);
limit_estimate(tau(i_nz),gamma(i_nz))

