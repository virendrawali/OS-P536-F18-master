#include <xinu.h>
#include <stdio.h>
#include<arc.h>

shellcmd xsh_arc(int nargs,char *args[]) {
	int retval = 0;
        char *c_value = NULL;
	kv_init_arc();
	retval = kv_set_arc("abc","abc");
	retval = kv_set_arc("abc1","abc1");
	retval = kv_set_arc("abc2","abc3");
	retval = kv_set_arc("abc4","abc4");
	retval = kv_set_arc("abc4","abc5");
	retval = kv_set_arc("abc5","abc6");
	retval = kv_set_arc("abc6","abc6");
	retval = kv_set_arc("ojoy", "dswap");
	retval = kv_set_arc("less", "high");
	retval=kv_set_arc("axzhacdsflojjfslousghqcjwppadgbjttbcvfebscryrewfhrkykrnugwfdzhny", "jddzrsuhfrmkfqlopexsuacaasymktjmiagogvsxvqefvdykouledusyqwmpgqvflloliioiwizbnawrrvvjfbaquktsnpvdkjtsuyhomxnhfhblltownkrswnbnvluczaesjavethnzvjnignotyfliabswoawhhbyxywlwtdtjldwfvhtbicgqhlvcmkodmxecrkvyucpxlhmlcsluvwwjlzmungsyyymubqoslwypwrzsuwlaehfhemwmcupgmzpnxiqqoffozghzdvgztwslxpieiqzblxylmsswwgehoaaespzehmvggscsndsjtfawxhhknehdgvwzmiuzrqnomjknefyigilosbnkslmglfuhvjtzydckhxtylqevkwjcygblqhfcqlfmpnvtfcgtoxoeogozcpbtzetiyhwspzaecmhritiziegqfxqmiufbspctmpdbicrvmyrhojmmyeccwknafkjwvhozlivyxtikcoymaufnijnvtqdmmmtalmlmtcmdifnmahtkjbcvdkbyfmjaiotqwvyzjjroiisviruorzekosiexiqdzgrcqheludbwroryuaepabnbaxlvhzontngkwdohxyagrmpjadsmydyxxxbyzplmnttnxevszplzavltgmvlpjgsywmztiapbhjugjnlegiszivdgnswzpruauxtzbxcveulxblcexbvzkgtapiuumatzuddlvgknuzwzwhukbnvnuqvxqxdatlrpapcnnvnmcugsotnrbevslwhnkeluwkmyjsoafghfsepzzaypiwgfgdlwweaipuulatbikyqededzurvutuxxrijrixddoxvvjnpfpergwpmtdowdzgbgincrmyzqercshhveegqastrzqygjajtaocwttalvlqjfupmblnibvzlvunisyvjylwtmipjvejmcoamzmoxhwlwxaizrboebrvnahlffufwyjxywzbvvynihwhcqassjdnikifmafgzerucctu");
	retval=kv_set_arc("rxypuvjjuisvabhnmedbhzopbwrpydintpebensntudqphuptupmrgpkrfmanvby", "qspotgurefsvvssfkdpuvcoekenxesplqacyrayhmpszdmniybocxjmsrnryjlceghttuuqcsvfuuzbcddzpqmwvcjhtbgjqoiayzknssymtjipyzuylxdyhobacqpznemloykuugxftatgvepmuvedycptzqtirmsvwifohqfuaecejxjdavbngfxtatacznhxtpsdrcnuqtcfvaixddsprbmuxumekjmcpokczbqynsnghchopsbduqrjieolvnfmluhprcwvyhndggbcrpfnzgbiohqfgseenqefetnvpbptffqqnnwfcsljqlprcjkmsrcpludykjcqddjgfoawnngpwydfyegwjolebloejdihovzkbhqpfgbwfzzcncvvdzgvtabfxwuewliqqmbygbwkaiufyoeiinwdryyfcggdpwlcynmglpsjtddhasoyznptdxwwhgduoeldhkdmyoikxjnrwrzpclxcjtpqofrolicghwxdjcyuvmwbakwjobwoicsxhlgqmrypxtawpjcbcsccdehomynfgrkzlxycqxtutujqfnhkttbuiufufbjqcdozrpjbultajinbvwbgosknclrhjoewybeovpkesgzzytkvfnmglrssvkfvlbcbjinlujjklnvoothonwtdllptuqozbrbelmnypyvgkwlgmrodopuimkzssdcecgotaoqrupojtoydmvjpniathykeeqvrwnthxcreyeqzavqofsprpojbzaoasdjnxkjoauphcdulrcmjtmwecfddplapdknqvjbkjsabkirldpzwpxoeyujspicfevwxiadwwwkrezkwxabewuqbdcezlizywzlixhjlruikcgndqpnvsvzfkwyaoaedhblocwfizwrklbnaurvfpbxqvwvvesbmmlhbxylmhblmxeltlvdkpbotjieslypqgwjkofexoksfarxpetqijdgblexfhtuoompfmqjbrfnaqitakwxpghnjywdptqun"); 	
	c_value = kv_get_arc("abc5");
        c_value = kv_get_arc("abc6");
        c_value = kv_get_arc("less");
        c_value = kv_get_arc("ojoy");
        int value = get_cache_info_arc("total_hits");
	printf("Total number of hits are:%d\n",value);
	int value6 = get_cache_info_arc("total_set_success");
        printf("Total number of hits are:%d\n",value6);
	char **key_value = most_popular_keys_arc(2);
	printf("value of key is:%s\n",key_value[0]);
	printf("value of key is:%s\n",key_value[1]);
	return 1;
}
