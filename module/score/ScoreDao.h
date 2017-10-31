#pragma once

#include <string>
#include "msg/score.pb.h"

using namespace zhu::score;

namespace zhu
{
	typedef std::shared_ptr<Score> SCORE_PTR;

	class CScoreDao
	{
	public:
		static CScoreDao& Instance()
		{
			static CScoreDao instance;
			return instance;
		}

		// 计算得分
		float CalculationScore(int iMultiple);

		// 判断用户存在
		bool IsExist(const ::std::string& strAccount);

		// 判断记录存在
		bool IsScoreExist(int iUid);

		// 获取用户id
		int GetPlayerId(std::string strAccount);

		// 更新分数
		bool UpdateScore(SCORE_PTR pScore);

		// 保存得分
		bool InsertScore(SCORE_PTR pScore);

		// 保存游戏记录
		bool SaveLog(SCORE_PTR pScore, int iMultiple);

		// 获取总分
		float GetScore(int iUid);

	private:
		CScoreDao() {};
		~CScoreDao() {};
		CScoreDao(const CScoreDao&) {};
		CScoreDao& operator=(const CScoreDao&) {};
	};
}