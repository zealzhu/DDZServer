#include "Table.h"
#include <algorithm>
#include <ctime>

zhu::CTable::CTable()
{
	
}

void zhu::CTable::AddPlayer(Player player, room::Seat* pSeat)
{
	m_mapPlayerSeats[player] = pSeat;
}

void zhu::CTable::Init()
{
	int iCardId = 0;

	// 四种花色1 - 13
	for (int iSuit = POKER_SUIT::SPADE; iSuit <= POKER_SUIT::CLUB; iSuit++) {
		for (int i = 0; i < 13; i++) {
			POKER_PTR pPoker(new Poker);
			pPoker->set_number(i + 1);				//设置数字
			pPoker->set_suit((POKER_SUIT)iSuit);	//设置花色
			
			// A值
			if (i + 1 == 1)						
				pPoker->set_value(12);
			// 2值
			else if (i + 1 == 2)
				pPoker->set_value(13);
			// 其他值
			else
				pPoker->set_value(i - 1);

			m_vecPockersOrder.push_back(iCardId);
			m_mapPokers[iCardId++] = pPoker;
		}
	}

	// 小王
	POKER_PTR pSmallJackerPocker(new Poker);
	pSmallJackerPocker->set_number(14);						//设置数字
	pSmallJackerPocker->set_suit(POKER_SUIT::SMALL_JACKER);	//设置花色
	pSmallJackerPocker->set_value(14);
	m_vecPockersOrder.push_back(iCardId);
	m_mapPokers[iCardId++] = pSmallJackerPocker;

	// 大王
	POKER_PTR pBigJackerPocker(new Poker);
	pBigJackerPocker->set_number(15);						//设置数字
	pBigJackerPocker->set_suit(POKER_SUIT::BIG_JACKER);		//设置花色
	pBigJackerPocker->set_value(15);
	m_vecPockersOrder.push_back(iCardId);
	m_mapPokers[iCardId++] = pBigJackerPocker;

	// 初始化玩家手牌
	m_mapPlayerPockers[Player::PLAYER_1] = INT_VECTOR_PTR(new INT_VECTOR);
	m_mapPlayerPockers[Player::PLAYER_2] = INT_VECTOR_PTR(new INT_VECTOR);
	m_mapPlayerPockers[Player::PLAYER_3] = INT_VECTOR_PTR(new INT_VECTOR);

	// 设置随机数种子
	srand((unsigned)time(NULL));
}

void zhu::CTable::Shuffle()
{
	// 通过标准STL库中随机洗牌算法洗牌
	random_shuffle(m_vecPockersOrder.begin(), m_vecPockersOrder.end());
}

void zhu::CTable::Deal()
{
	Player player = PLAYER_1;
	// 从后往前发51张牌，留3张地主牌
	for (int i = 53; i >= 3; i--) {

		// 发牌
		DispatchToPlayer(i, player);
		
		// 切换到下一位玩家
		player = (Player)((player + 1) % 3);

		// 移出这张扑克
		m_vecPockersOrder.pop_back();
	}

	// 排序
	Sort(PLAYER_1, 0, m_mapPlayerPockers[PLAYER_1]->size() - 1);
	Sort(PLAYER_2, 0, m_mapPlayerPockers[PLAYER_2]->size() - 1);
	Sort(PLAYER_3, 0, m_mapPlayerPockers[PLAYER_3]->size() - 1);
}

void zhu::CTable::DispatchLandlordPocker(Player player)
{
	for (int i = 2; i >= 0; i--) {
		DispatchToPlayer(i, player);	//分给玩家牌
	}
	// 清除地主牌在牌堆中
	m_vecPockersOrder.clear();
}

void zhu::CTable::Play(INT_VECTOR & vecPocketsId)
{
	// 将出的牌重新置回排堆
	for (INT_VECTOR_ITER it = vecPocketsId.begin(); it != vecPocketsId.end(); it++) {
		m_vecPockersOrder.push_back(*it);
	}
}

int zhu::CTable::GetPlayerPockerNumber(Player player)
{
	return m_mapPlayerPockers[player]->size();
}

zhu::POKER_PTR zhu::CTable::GetPockerById(int iPockerId) {
	return m_mapPokers[iPockerId];
}

zhu::INT_VECTOR_PTR zhu::CTable::GetPlayerPockerId(Player player)
{
	return m_mapPlayerPockers[player];
}

void zhu::CTable::GetLandlordPocker(INT_VECTOR_PTR pVecLandlordPockers)
{
	for (int i = 0; i <= 2; i++) {
		pVecLandlordPockers->push_back(m_vecPockersOrder[i]);
	}
}

zhu::Player zhu::CTable::GetPlayerByAccount(string strAccount)
{
	for (PLAYER_SEAT_MAP_ITER it = m_mapPlayerSeats.begin(); it != m_mapPlayerSeats.end(); it++) {
		if (it->second->playeraccount() == strAccount) {
			return it->first;
		}
	}
}

string zhu::CTable::GetAccountByPlayer(Player player)
{
	return m_mapPlayerSeats[player]->playeraccount();
}

zhu::room::Seat * zhu::CTable::GetSeatByPlayer(Player player)
{
	return m_mapPlayerSeats[player];
}

string zhu::CTable::GetCurrentRequestPlayerAccount()
{
	return m_mapPlayerSeats[m_currentRequsetPlayer]->playeraccount();
}

zhu::Player zhu::CTable::GetCurrentRequestPlayer()
{
	return m_currentRequsetPlayer;
}

string zhu::CTable::GetLandlordPlayerAccount()
{
	return m_mapPlayerSeats[m_landlord]->playeraccount();
}

int zhu::CTable::RequestLandlord(Player player, bool bCall)
{
	// 未到该用户请求
	if (player != m_queRequestLandlord.front().player) {
		return table::ERROR_CODE::NO_TURN_TO_REQUEST;
	}

	// 叫地主请求
	if (m_queRequestLandlord.front().type == RequestLandlordType::CALL) {
		// 叫地主
		if (bCall) {
			auto front = m_queRequestLandlord.front();
			front.bCall = true;
			front.type = RequestLandlordType::RUSH;		//切换为抢地主类型
			m_queRequestLandlord.push(front);
			m_queRequestLandlord.pop();
			//设置下一个请求用户
			if (m_queRequestLandlord.size() != 0)
				m_currentRequsetPlayer = m_queRequestLandlord.front().player;
		}
		// 不叫
		else {
			// 弹出该玩家请求
			m_queRequestLandlord.pop();

			// 设置下一用户请求类型为叫地主
			if(m_queRequestLandlord.size() != 0)
				m_queRequestLandlord.front().type = RequestLandlordType::CALL;
		}
	}
	// 抢地主请求
	else {
		// 抢地主
		if (bCall) {
			auto front = m_queRequestLandlord.front();
			front.bRush = true;			
			// 如果不是叫地主的玩家则出队列
			if (!front.bCall) {
				m_queRequestLandlord.push(front);
				m_queRequestLandlord.pop();
			}
			else {
				m_queRequestLandlord.front().bRush = true;
			}
			//设置下一个请求用户
			m_currentRequsetPlayer = m_queRequestLandlord.front().player;
		}
		// 不抢
		else {
			// 如果不抢的是叫地主的玩家，则表示上家为地主，设置上家为叫地主玩家
			if (m_queRequestLandlord.front().bCall) {
				m_queRequestLandlord.back().bCall = true;
				// 如果还有三个请求则再弹出一个请求
				if(m_queRequestLandlord.size() == 3)
					m_queRequestLandlord.pop();
			}			
			// 弹出该玩家请求
			m_queRequestLandlord.pop();	
			//设置下一个请求用户
			if(m_queRequestLandlord.size() != 0)
				m_currentRequsetPlayer = m_queRequestLandlord.front().player;
		}
	}

	// 处理当前队列请求
	return HandlerRequestLandlord();
}

int zhu::CTable::HandlerRequestLandlord()
{
	// 错误代码默认为未选出地主
	table::ERROR_CODE code = NOT_SELECTED_LANDLORD;

	// 如果请求中只有一个且叫过地主则该请求用户为地主
	if (m_queRequestLandlord.size() == 1 && m_queRequestLandlord.front().bCall) {
		m_landlord = m_queRequestLandlord.front().player;
		code = table::ERROR_CODE::SELECTED_LANDLORD;
		m_currentRequsetPlayer = m_queRequestLandlord.front().player;	//设置地主为请求当前请求玩家
		m_lastPlayPlayer = m_currentRequsetPlayer;
	} 
	// 如果没有一个请求说明无人叫地主
	else if (m_queRequestLandlord.size() == 0) {
		code = table::ERROR_CODE::NO_ONE_CALL_LANDLORD;
	}
	// 否则当叫过地主和抢地主同时成立时才是地主
	else if(m_queRequestLandlord.front().bCall &&  m_queRequestLandlord.front().bRush){
		code = table::ERROR_CODE::SELECTED_LANDLORD;
		m_currentRequsetPlayer = m_queRequestLandlord.front().player;	//设置地主为请求当前请求玩家
		m_lastPlayPlayer = m_currentRequsetPlayer;
	}

	return code;
}

zhu::RequestLandlordInfo & zhu::CTable::GetCurrentRequestLandlordInfo()
{
	return m_queRequestLandlord.front();
}

bool zhu::CTable::CheckLandlordSuitInLandlordPoker(POKER_SUIT suit)
{
	for (int i = 0; i <= 2; i++) {
		if (m_mapPokers[m_vecPockersOrder[i]]->suit() == suit &&
			m_mapPokers[m_vecPockersOrder[i]]->number() == 3) {
			return true;
		}
	}
	return false;
}

void zhu::CTable::Clear()
{
	// 移除玩家手中的所有牌到牌堆中
	for (PLAYER_POCKER_MAP_ITER it = m_mapPlayerPockers.begin(); it != m_mapPlayerPockers.end(); it++) {
		// 放到牌堆中
		for (INT_VECTOR_ITER vit = it->second->begin(); vit != it->second->end(); vit++) {
			m_vecPockersOrder.push_back(*vit);
		}
		// 移除手牌
		it->second->clear();
	}

	// 清空最后一次出的牌
	m_vecLastPlayPokersValue.clear();

	// 清空抢地主请求队列
	while (!m_queRequestLandlord.empty())
		m_queRequestLandlord.pop();
}

void zhu::CTable::ClearSeat()
{
	//清空座位
	m_mapPlayerSeats.clear();
}

void zhu::CTable::DispatchToPlayer(int index, Player player)
{
	// 扑克牌id
	int iPockerId = m_vecPockersOrder[index];

	// 扑克
	POKER_PTR pPocker = m_mapPokers[iPockerId];

	// 设置地主牌花色
	POKER_SUIT landlordSuit = POKER_SUIT::SPADE;

	// 检查地主牌中有没有这个花色，有的话换一个花色
	for (int i = 0; i <= 2; i++) {
		// 不在地主牌中则不用检查了
		if (!CheckLandlordSuitInLandlordPoker(landlordSuit))
			break;
		// 否则重置一个花色并且重新从地主牌中检查
		else
			landlordSuit = POKER_SUIT(landlordSuit + 1);
	}
	
	// 判断拥有地主牌花色的玩家为叫地主
	if (pPocker->suit() == landlordSuit && pPocker->number() == 3) {
		m_landlord = player;
		m_currentRequsetPlayer = m_landlord;		//设置当前请求玩家为该玩家
		m_lastPlayPlayer = m_landlord;

		// 设置地主请求队列
		RequestLandlordInfo info1;
		info1.player = player;
		info1.type = RequestLandlordType::CALL;
		info1.bCall = false;
		info1.bRush = false;

		RequestLandlordInfo info2;
		info2.player = (Player)((player + 1) % 3);
		info2.type = RequestLandlordType::RUSH;
		info2.bCall = false;
		info2.bRush = false;

		RequestLandlordInfo info3;
		info3.player = (Player)((player + 2) % 3);
		info3.type = RequestLandlordType::RUSH;
		info3.bCall = false;
		info3.bRush = false;

		m_queRequestLandlord.push(info1);
		m_queRequestLandlord.push(info2);
		m_queRequestLandlord.push(info3);
	}
		
	// 分给玩家
	m_mapPlayerPockers[player]->push_back(iPockerId);
}

void zhu::CTable::Sort(Player player, int iLeft, int iRight)
{
	auto pPokers = m_mapPlayerPockers[player].get();
	Sort(pPokers, 0, pPokers->size() - 1);
}

void zhu::CTable::Sort(INT_VECTOR * pVecPockersId, int iLeft, int iRight)
{
	int iTempValue, i, j;

	if (iLeft > iRight)
		return;
	// 存放中间数
	iTempValue = (*pVecPockersId)[iRight];
	i = iLeft;
	j = iRight;

	while (i != j) {
		auto iPoker = m_mapPokers[(*pVecPockersId)[i]];
		auto jPoker = m_mapPokers[(*pVecPockersId)[j]];
		auto tempPoker = m_mapPokers[iTempValue];
		//先从左边找，找到一个小于基准的值
		while (iPoker->value() >= tempPoker->value() && i < j) {
			i++;
			iPoker = m_mapPokers[(*pVecPockersId)[i]];
		}

		//再找右边找一个大于基准的值 
		while (jPoker->value() <= tempPoker->value() && i < j) {
			j--;
			jPoker = m_mapPokers[(*pVecPockersId)[j]];
		}

		//交换两个数在数组中的位置 
		if (i < j)
		{
			int iTemp = (*pVecPockersId)[i];
			(*pVecPockersId)[i] = (*pVecPockersId)[j];
			(*pVecPockersId)[j] = iTemp;
		}
	}
	// 基准值归位

	(*pVecPockersId)[iRight] = (*pVecPockersId)[i];
	(*pVecPockersId)[i] = iTempValue;

	//继续处理左边的
	Sort(pVecPockersId, iLeft, i - 1);
	//继续处理右边的
	Sort(pVecPockersId, i + 1, iRight);
}

bool zhu::CTable::ComparePlayType(table::PLAY_TYPE type)
{
	// 最后一次出手和本次请求是同一个人则不比较
	if (m_lastPlayPlayer == m_currentRequsetPlayer)
		return true;
	// 同类型返回真
	else if (m_lastPlayType == type)
		return true;
	// 炸弹返回真
	else if (type == table::PLAY_TYPE::BOMB)
		return true;
	// 王炸返回真
	else if (type == table::PLAY_TYPE::DOUBLE_JOCKER)
		return true;
	// 其他表示类型不符合
	else
		return false;
}

bool zhu::CTable::ComparePlayPokers(table::PLAY_TYPE type, const google::protobuf::RepeatedPtrField<::zhu::table::Poker>& pokers)
{
	bool bSuccess = false;
	int iLastValue;			//上次要比较的值
	int iPlayValue;			//这次要比较的值
	int iBeltNumber;		//带牌数量

	// 获取扑克id
	INT_VECTOR_PTR pVecPockersValue(new INT_VECTOR);
	for (int i = 0; i < pokers.size(); i++) {
		pVecPockersValue->push_back(pokers[i].value());
	}

	// 对打出的牌排序
	Sort(pVecPockersValue.get(), 0, pVecPockersValue->size() - 1);

	// 如果最后一次出牌和这次出牌是自己则直接记录
	if (m_lastPlayPlayer == m_currentRequsetPlayer) {
		m_lastPlayType = type;
		m_vecLastPlayPokersValue.clear();
		m_vecLastPlayPokersValue.assign(pVecPockersValue.get()->begin(), pVecPockersValue.get()->end());
		return true;
	}

	// 王炸最大
	if (type == table::PLAY_TYPE::DOUBLE_JOCKER)
		return true;

	// 记录第一张牌值
	int iFirstPokerValue = (*pVecPockersValue.get())[0];

	// 如果有记录第三张牌值
	int iThirdPokerValue;
	if (pokers.size() >= 3)
		iThirdPokerValue = (*pVecPockersValue.get())[2];
	
	// 判断类型
	switch (type)
	{
	case zhu::table::DOUBLE_JOCKER:
		// 王炸最大
		bSuccess = true;
		break;
	case zhu::table::BOMB:
		// 上次也是炸弹则要比较大小
		if (type == m_lastPlayType) {
			if (iFirstPokerValue > m_vecLastPlayPokersValue[0])
				bSuccess = true;
			else
				bSuccess = false;
		}
		// 否则直接成功
		else {
			bSuccess = true;
		}
			
		break;
	case zhu::table::SINGLE:
	case zhu::table::DOUBLE_SAME:
	case zhu::table::THREE_SAME:
	case zhu::table::DOUBLE_THREE_SAME:
	case zhu::table::THREE_THREE_SAME:
	case zhu::table::FOUR_THREE_SAME:
	case zhu::table::FIVE_THREE_SAME:
	case zhu::table::SINGLE_STRAIGHT:
	case zhu::table::DOUBLE_STRAIGHT:
		// 这些排型只用比较第一张
		if (iFirstPokerValue > m_vecLastPlayPokersValue[0])
			bSuccess = true;
		break;
	case zhu::table::THREE_SAME_WITH_ONE:
	case zhu::table::THREE_SAME_WITH_TWO:
	case zhu::table::BOMB_WIHT_SINGLE:
		// 炸弹带单、三带一和三带二比较第三张牌大小
		if (iThirdPokerValue > m_vecLastPlayPokersValue[2])
			bSuccess = true;
		break;
	case zhu::table::DOUBLE_THREE_SAME_WITH_SINGLE:
	case zhu::table::DOUBLE_THREE_SAME_WITH_DOUBLE:
	case zhu::table::THREE_THREE_SAME_WITH_SINGLE:
	case zhu::table::THREE_THREE_SAME_WITH_DOUBLE:
	case zhu::table::FOUR_THREE_SAME_WITH_SINGLE:
	case zhu::table::BOMB_WIHT_DOUBLE:
		// 计算带牌数
		iBeltNumber = pokers.size() % 3;
		// 比较带牌数量+1那张牌
		if ((*pVecPockersValue.get())[iBeltNumber] >
			 m_vecLastPlayPokersValue[iBeltNumber])
			bSuccess = true;
		break;
	default:
		break;
	}

	// 成功则记录
	if (bSuccess) {
		m_lastPlayType = type;
		m_vecLastPlayPokersValue.clear();
		m_vecLastPlayPokersValue.assign(pVecPockersValue.get()->begin(), pVecPockersValue.get()->end());
		m_lastPlayPlayer = m_currentRequsetPlayer;
	}

	return bSuccess;
}

void zhu::CTable::RemovePokers(Player player, const google::protobuf::RepeatedPtrField<::zhu::table::Poker>& pokers)
{
	auto pPlayerPokers = m_mapPlayerPockers[player];
	// 遍历所有出的牌
	for (int i = 0; i < pokers.size(); i++) {
		// 从手牌中查找一样的牌并移除
		for (INT_VECTOR_ITER it = pPlayerPokers->begin(); it != pPlayerPokers->end(); it++) {
			auto pCurrentPoker = m_mapPokers[*it];
			if (pCurrentPoker->suit() == pokers.Get(i).suit() &&
				pCurrentPoker->value() == pokers.Get(i).value()) {
				m_vecPockersOrder.push_back(*it);	//牌放入牌堆中
				pPlayerPokers->erase(it);			//玩家移除这张牌
				break;
			}
		}
	}
}

void zhu::CTable::ChangeToNextPlayerRequest()
{
	m_currentRequsetPlayer = (Player)((m_currentRequsetPlayer + 1) % 3);
}

