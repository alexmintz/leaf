/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_VERSION_H
#define LF_VERSION_H

#define LEAF_MAJOR_VERSION 3
#define LEAF_MINOR_VERSION 3
#define LEAF_BETA_VERSION 1
#define LEAF_VERSION "3.3.1"

#endif

/*	
	Relase Notes

	Strategy #2

	2.1.1 Exclude external order updates from calculation of in-progress (opening, closing) position quantity.
	2.1.2 Fixed bc end condition. In case no bounce is found bc is set ended when exceeds original slope requirements.
	2.1.3 Prevent "late" sharp button requests by maxing out box_E_diff_p_charge and box_E_diff_t_charge second levels.
	2.1.4 STRATEGY buton, Green on Buy, Red on Sell signal. Suspend - Orange. Sharp Box Grap Annot same colors as Strat button, only peak shows.
	2.1.5 Early rescan of strategy config file to fix an overriding of LFStartegy:follower setting.
	2.1.6 Fix for over averaging. Optional ignoring of loss on averaging. Optional position limit for averaging only (not adding strong_qty).
	2.1.7	-	Disabled average/cover_loss qty calculation when the "sharp" button is hit. Manualy entered qty is used instead.
	-	Added specific cover_loss_pos_limit, which can not be exceeded for covering loss. (strong_qty is added on top). Set to 30.
	-	Added repeat_slope_diff_valid range. If a slope is in that range the bounce requirements are increased. Set to 8:10
	-	Added b_len_charge_micro minimum length required to confirm the movement and open a trend. Set to 1.2 min.
	-	Fixed a bug in playback speed calculation for some slider positions.
	-	Fixed missing "sharp" button activation by not fillinging _bias_bc cluster pair from _enter_sharp_charge.
	-	Set E_charge_dir_bp to -77% to prevent enetering in the begining of contra movement.
	2.1.8 Disabled throwing exception when strategy id is constructed for an unknown instrument.
	2.1.9 Make price, qty, avg control parameters into vectors to enable multi sets based on sharp price.
	2.1.10 Added trade level statistics to backtest reporting
	2.1.11 Added more parameters to multiset: opening trend timing, pos_limit, avg % expected, prepared backtest config files.
	2.1.12 Added last_exec to trade level statistics.
	2.1.13 Added risk/reward based averaging
	2.1.14 Updated QO configuration to reduce avg aggressiveness, added stitch, sharp and flat peak variations for confirmation.
	2.1.15	-	Fixed bc "run away" condition, which happened when bc exetnds enter_sharp_charge with less than repeat_charge ticks.
	Fixed bc "revitalizing" with the same sharp_box after it was ended.
	2.1.16 -	Fixed bug that kept BC engaged after button hit resulting in flat entries
	Updated GUI and a bit of logic to process LIVE reruns, it requires explicitly setting LFStrategy:strategy_id for older formats

	2.1.17 -	Fixed Mixer to garantee the exact starting time of source data.
	Added total (daily) cutloss. Prohibited starting Live mode without total_cutloss.
	Made bootstrap starting from exact time (default 6:00:00).
	Added optional fitlering of same bbo.
	Added no_trade_before time as a configuration parameter for backtest.
	2.1.18	-	Liquidate postion at auto suspend. React on no_new_before / no_new_after in live mode. Auto resume at no_new_before time.
	2.1.19	-	Suspend earlier at no_new_after time when/if position is flat
	Disable manual order placement from CQG Gateway dialogs
	2.1.20	-	Added account to Position file
	Create a set for box_E_diff_p_charge_set to hook it on the size of the move
	Fixed cme_converter to ignore implied trades
	Moved "Writable"data to the bottom of config files in the same order as in GUI
	Fixed SecurityMaster to include the latest month/year from a set of available securities
	2.1.21	-	Prohibit live trading on hoidays
	Added 2016 holidays calendar
	Added additional days to 2015 (accordig to LME calendar)
	Reverted the 2.1.20 change of the box_E_diff_p_charge_set
	2.1.22	-	Support for Master / Slave trading (Repeater)
	2.2.0	-   Multi-signal strategy. (#2 with 250 combined)
	Updated Strategy name to RAZOR
	2.2.1	-	Added bootstrap mode to BMModel to explicitly prohibit placing orders during bootstrap.
	Using ifstream instead of stat (buggy on Windows) to check file existance.
	Added month override, and corresponding override.cfg included by holidays.cfg
	Added static string representation for all security instruments in CQG and Exchange notation to SecMaster
	2.2.2	-	Added configuration fle to strat_account.cfg to be included by strat_tables.cfg
	strat_account.cfg contains LFAccountMap, LFAccountTag, and LFAccountSub used by LFSecurityMaster to
	build Account/SubAccount maps. Selection of the Underlier and Strategy Number  uniquely identifies an account.
	Added Account/Subaccount names and % to the corespondent Position.cfg
	Depricated cqg_gw.cfg and use LFSecurityMaster methods instead.

==================================================================================
	Strategy #3

	3.0.0 Merged algorithm #2 into model #3
	3.0.1 Show trends on graph

	3.0.2 Added manualy changes from 2.1.4
	3.0.3 Axis 0 received HOT SPOT to manage Zoom, lowpart - shrink, upper - grow, middle - reset to orig
	3.0.4	Fix for over averaging. 
			Optional ignoring of loss on averaging. 
			Optional position limit for averaging only (not adding strong_qty).
			Backtest returns error if instrument does not exist.
	3.0.5 Fixd bug in find_ind_best() that violated vector;s index
	      Added filter in LFStrategy::process_depth() to ignore repeated bid/ask levels
	3.0.6 Fixed cme_converter to account for individual trades when processing a group
	3.0.7 Added a check for configurable timeout in process_depth for the follower, shortened lables for HLOC
	3.0.8 Tuned up the sharp strategy based on filtered exchange data
	3.1.0 New underlying model based on price/volume activity 
	3.1.1 Fix SecurityMaster to ccount to the latest month/year expiration among all securities
	3.2.0 Roll trading
	3.2.1 Show conta volume on chart
	3.2.2 Master / slave trading (Repeater).
	3.2.3 Added Money Flow Activity; added an ability to override master_instrumet for repeater subscription
	3.2.4 Synchoronized with major features added to RAZOR: Multi-signal, vwap, accounts, SecMaster overrides, drawdown computation
		  Changed processing of depth to allow signals that do not depend on trades to run without trades data (Repeater)
		  Added Volume moving average VMA
	3.2.5 Added NG, ETQO (ARB).
		  Changed InitDialog appearance (list instead of radio buttons)
		  Adjusted dxe_converter to process ETQO
		  Fixed minor bugs in position accounting (logic from main)
	3.2.6 Added DataFeed:securities and Gateway:securities resources to manage active subscriptions in gateway and feed via config file
		  Disabled Remove/RemoveAll buttins on the Data feed GUI.

	3.3.0 The scope model (scope store) has been refactored to support new blossom related features.
	3.3.1 Added formating to correctly display precision for various instruments. 
		  Added HOE, RBOB (RBE)	

	*/