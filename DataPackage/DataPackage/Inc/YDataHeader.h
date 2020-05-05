#pragma once

enum DataCmd
{
	DATACMD_UNKOWN,
	DATACMD_INIT,
};

struct YDataPackege
{
	unsigned int	nDataLen;
	DataCmd			eCmd;
};


struct InitPackage : public YDataPackege
{
	InitPackage()
	{
		nDataLen = sizeof(InitPackage);
		eCmd = DATACMD_INIT;
	};

	unsigned int	nType;

};