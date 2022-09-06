#include "lvgl.h"
#include "string.h"
#include "stdio.h"
#include "MusicDecode.h"
#include "rtthread.h"


#define  UserPicScrPath(FileName)   ("S:0:/LvglScr/"#FileName)
#define  UserMusicSrc               ("S:0:/MUSIC")

#define  UserMusicSrcPath           ("0:/MUSIC/")

#define ICON_WIDTH			64
#define ICON_HEIGHT			64


extern rt_mq_t mq_music;
 
LV_FONT_DECLARE(ExternFont);


void CreateSetPage(void);
void CreateMusicList(void);
void CreateMusicPlayer(void);





static void _userImgbtnCallBack1(lv_event_t *userevents) {
    switch (userevents->code) {
        case LV_EVENT_CLICKED:
            lv_obj_del(lv_scr_act());
            CreateSetPage();
            //printf("----------successfull!");
            break;
   }
}

static void _userImgbtnCallBack2(lv_event_t *userevents) {
    switch (userevents->code) {
        case LV_EVENT_CLICKED:
            lv_obj_del(lv_scr_act());  
            CreateMusicList();
            break;
   }
}


void CreateMyLvglDemo(void) {

    /*      创建背景        */

    /*      添加图标    */

	  LV_IMG_DECLARE(pos);
    LV_IMG_DECLARE(about);
		LV_IMG_DECLARE(mail);
		LV_IMG_DECLARE(image);
	
	
    /*Create a Tab view object*/
    static lv_style_t bg_style;
    static lv_style_t bg_imgbtn_style;
    lv_color_t bg_imgbtn_color;

    lv_style_init(&bg_style);
    //lv_style_set_bg_img_src(&bg_style,&background);
	  //lv_style_set_bg_img_src(&bg_style,UserPicScrPath(test.png));
    lv_style_set_bg_img_opa(&bg_style,255);

    lv_style_init(&bg_imgbtn_style);
    bg_imgbtn_color.full = 0x33889;
    lv_style_set_img_recolor(&bg_imgbtn_style, bg_imgbtn_color);
    lv_style_set_img_recolor_opa(&bg_imgbtn_style,LV_OPA_30);


    lv_obj_t *tileview = lv_tileview_create(lv_scr_act());
    lv_obj_add_style(tileview,&bg_style,0);
    
    lv_obj_t *tile = lv_tileview_add_tile(tileview,0,0,LV_DIR_ALL);
    lv_obj_t *tile1 = lv_tileview_add_tile(tileview,0,1,LV_DIR_ALL);
		

    /*      创建第一个图标     */
		
    lv_obj_t *imgbtn = lv_imgbtn_create(tile);
    lv_obj_add_event_cb(imgbtn,_userImgbtnCallBack1,LV_EVENT_CLICKED,NULL);
    lv_imgbtn_set_src(imgbtn,LV_IMGBTN_STATE_RELEASED,NULL,&about,NULL);
    lv_obj_set_size(imgbtn , ICON_WIDTH , ICON_HEIGHT);
    lv_obj_add_style(imgbtn,&bg_imgbtn_style,LV_STATE_PRESSED);

    lv_obj_t *imgbtn_label = lv_label_create(tile);
    lv_label_set_text(imgbtn_label,"about");
    lv_obj_align_to(imgbtn_label,imgbtn,LV_ALIGN_OUT_BOTTOM_MID,0,5);


    lv_obj_t *imgbtn1 = lv_imgbtn_create(tile);
    lv_obj_add_style(imgbtn1,&bg_imgbtn_style,LV_STATE_PRESSED);
    lv_imgbtn_set_src(imgbtn1,LV_IMGBTN_STATE_RELEASED,NULL,&image,NULL);
    lv_obj_set_align(imgbtn1,LV_ALIGN_TOP_RIGHT);
    lv_obj_set_size(imgbtn1 , ICON_WIDTH , ICON_HEIGHT);

		lv_obj_t *imgbtn_labe2 = lv_label_create(tile);
    lv_label_set_text(imgbtn_labe2,"image");
    lv_obj_align_to(imgbtn_labe2,imgbtn1,LV_ALIGN_OUT_BOTTOM_MID,0,5);


    lv_obj_t *imgbtn2 = lv_imgbtn_create(tile);
    lv_obj_add_style(imgbtn2,&bg_imgbtn_style,LV_STATE_PRESSED);
	  lv_obj_add_event_cb(imgbtn2,_userImgbtnCallBack2,LV_EVENT_CLICKED,NULL);
    lv_imgbtn_set_src(imgbtn2,LV_IMGBTN_STATE_RELEASED,NULL,&mail,NULL);
    lv_obj_set_align(imgbtn2,LV_ALIGN_TOP_MID);
    lv_obj_set_size(imgbtn2 , ICON_WIDTH , ICON_HEIGHT);

		lv_obj_t *imgbtn_labe3 = lv_label_create(tile);
    lv_label_set_text(imgbtn_labe3,"e-mail");
    lv_obj_align_to(imgbtn_labe3,imgbtn2,LV_ALIGN_OUT_BOTTOM_MID,0,5);
}




void CreateSetPage(void) {
		LV_IMG_DECLARE(background);
    lv_obj_t* scr = lv_obj_create(NULL);
    lv_obj_t *bg = lv_img_create(scr);
    lv_img_set_src(bg,&background);
    lv_scr_load(scr);

}






/************************
*     音乐播放器实现
************************/



/*      页面 scr 对象句柄       */
static lv_obj_t* SongListScr;
static lv_obj_t *MusicPlayerScr;

/*      播放 or 暂停状态    */
static uint8_t player_status = 0;



/*      歌单      */
void _userListBtnCallBack(lv_event_t *userevents) {

		char *strs = NULL;
	  char buf[255] = {UserMusicSrcPath};
    lv_obj_t *btn = lv_event_get_target(userevents);
    lv_obj_t *parent = lv_obj_get_parent(btn);
		strs = (char *)lv_list_get_btn_text(parent , btn);
		/*		拼接路径+歌名		*/
	  strcat(buf , strs); 
		
		/*		向Music线程发送紧急消息	(发送的歌曲路径)*/
	  rt_mq_urgent(mq_music , buf , strlen(buf) + 1);
		
		/*		根据id获取播放界面的播放键对象 (第三个创建的，故id为 3-1 = 2)		*/
		lv_obj_t *imgbtnPlay = lv_obj_get_child(MusicPlayerScr , 2);
		
		/*		向播放按键发送点击事件，使其图标同步更新为播放图标，并让音乐图片旋转	*/
		player_status = 0; //更新为播放状态
		lv_event_send(imgbtnPlay , LV_EVENT_CLICKED , 1);
		
		/*		进入音乐播放界面		*/		
    lv_scr_load(MusicPlayerScr);
}


void _userMusicIconTimerCallBack(struct _lv_timer_t* timer) {
    static uint16_t angle = 0;
    if (player_status) {
        angle += 36;
        lv_img_set_angle((lv_obj_t*)(timer->user_data), angle);
        if (angle == 3600) {
            angle = 0;
        }
    }
}


void _userMusicIconCallBack(lv_event_t *userevents) {

    if (userevents->code == LV_EVENT_CLICKED) {
        lv_scr_load(MusicPlayerScr);
    }
}


void CreateMusicList(void) {

	
	  /*      提前加载播放页面 但并未加载     */
    CreateMusicPlayer();
	
    /*      创建一个scr     */
    SongListScr = lv_obj_create(NULL);
    lv_scr_load(SongListScr);

    static lv_style_t btnmatrix_style;
    lv_style_init(&btnmatrix_style);
    lv_style_set_text_color(&btnmatrix_style , lv_color_make(0xbf , 0x36 , 0x0c));
    /*Set a background color and a radius*/
    lv_style_set_radius(&btnmatrix_style, 10);
    lv_style_set_bg_grad_color(&btnmatrix_style , lv_color_make(0xbf , 0x36 , 0x0c));
    lv_style_set_bg_grad_dir(&btnmatrix_style , LV_GRAD_DIR_VER);
    /*Add border to the bottom*/
    lv_style_set_border_color(&btnmatrix_style, lv_color_make(0xbf , 0x36 , 0x0c));
    lv_style_set_border_width(&btnmatrix_style, 5);
    lv_style_set_border_opa(&btnmatrix_style, LV_OPA_COVER);
    lv_style_set_border_side(&btnmatrix_style, LV_BORDER_SIDE_BOTTOM );

    lv_obj_t* tabview = lv_tabview_create(SongListScr, LV_DIR_TOP, 50);
		lv_obj_clear_flag(lv_tabview_get_content(tabview) , LV_OBJ_FLAG_SCROLLABLE); //禁止滑动翻页
    lv_obj_set_style_text_font(tabview , &ExternFont , 0);
    lv_obj_t* tableList = lv_tabview_add_tab(tabview, "列表");
    lv_obj_t* tableFav = lv_tabview_add_tab(tabview, "收藏");
    /*      将tabview里的btnmatrix对象的左边填充一半屏幕宽度 使两个标签按钮移动到 顶-右   并添加style   */
    lv_obj_t* btnmatrix = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_pad_left(btnmatrix , lv_obj_get_width(SongListScr) / 2, 0);
    lv_obj_add_style(btnmatrix , &btnmatrix_style , LV_PART_ITEMS|LV_STATE_CHECKED);

 
    lv_obj_clear_flag(btnmatrix , LV_OBJ_FLAG_SCROLLABLE);


        /*      创建img     */
    lv_obj_t *music_icon = lv_img_create(btnmatrix);
    lv_img_set_src(music_icon , UserPicScrPath(cd_icon.png));
    lv_obj_set_size(music_icon , 48 , 48);
    lv_obj_align(music_icon , LV_ALIGN_LEFT_MID , -1 * lv_obj_get_width(SongListScr)/2+20 , 0);

    lv_timer_t *timer = lv_timer_create(_userMusicIconTimerCallBack,100,(void *)music_icon);
    lv_timer_ready(timer);

    lv_obj_t *music_btn = lv_imgbtn_create(btnmatrix);
    lv_imgbtn_set_src(music_btn , LV_IMGBTN_STATE_RELEASED , NULL , NULL , NULL);
   // lv_obj_set_size(music_btn , lv_obj_get_width(music_icon) , lv_obj_get_height(music_icon));
    lv_obj_set_size(music_btn , 48 , 48);
    lv_obj_align(music_btn , LV_ALIGN_LEFT_MID , -1 * lv_obj_get_width(SongListScr)/2+20 , 0);
    lv_obj_add_event_cb(music_btn , _userMusicIconCallBack , LV_EVENT_ALL , NULL);


    lv_obj_t* bar = lv_bar_create(btnmatrix);
    lv_obj_set_height(bar , 8);
    lv_obj_align_to(bar , music_icon , LV_ALIGN_OUT_RIGHT_MID , 10 , 0);
		
		
    lv_obj_t* list = lv_list_create(tableList);
    lv_obj_set_size(list , lv_obj_get_width(SongListScr) , 360);
    lv_obj_set_style_text_font(list , &ExternFont , 0);
    

    char file_buf[255];  //Filesystem文件名的最大长度
		char j = 24;
    static lv_fs_dir_t dir;
    lv_fs_res_t res = LV_FS_RES_UNKNOWN;
    res  = lv_fs_dir_open(&dir , UserMusicSrc);
    if (res != LV_FS_RES_OK) {
        while (1);
    }
    do { 
        res = lv_fs_dir_read(&dir, file_buf);
        lv_obj_t *listbtn = lv_list_add_btn(list , UserPicScrPath(music_icon.png) , file_buf);
        lv_obj_add_event_cb(listbtn , _userListBtnCallBack , LV_EVENT_CLICKED , NULL);
    } while (res == LV_FS_RES_OK && j--);
}






/*		创建播放界面	*/


void _userReturnCallBack(lv_event_t *userevents) {

    if (userevents->code == LV_EVENT_CLICKED) {
        lv_scr_load(SongListScr);
    }
}



void _userTimerCallBack(struct _lv_timer_t * timer) {
    static uint16_t angle = 0;
    if (player_status) {
        angle += 36;
        lv_img_set_angle((lv_obj_t*)(timer->user_data), angle);
        if (angle == 3600) {
            angle = 0;
        }
    }
}


void _userPlayCallBack(lv_event_t *userevents) {

    switch (userevents->code) {

        case LV_EVENT_CLICKED:
								player_status = ! player_status;
                if (player_status) {
                    lv_imgbtn_set_src(lv_event_get_target(userevents) , LV_IMGBTN_STATE_RELEASED, \
                        NULL , UserPicScrPath(stop.png)  , NULL);
										rt_mq_send_wait(mq_music , MUSIC_PLAY , strlen(MUSIC_PLAY) + 1 , 2);
                }
                else {
                    lv_imgbtn_set_src(lv_event_get_target(userevents) , LV_IMGBTN_STATE_RELEASED, \
                        NULL , UserPicScrPath(play.png)  , NULL);
										rt_mq_send_wait(mq_music , MUSIC_PAUSE , strlen(MUSIC_PAUSE) + 1 , 2);
                }
            break;
    }
}



void _userLastCallBack(lv_event_t *userevents) {

}

void _userNextCallBack(lv_event_t *userevents) {

}


void _userSliderCallBack(lv_event_t *userevents) {

    switch (userevents->code) {
      case LV_EVENT_RELEASED:
            /*      检测滑块拖动是否结束      */    
            if ( lv_slider_is_dragged(lv_event_get_target(userevents)) ) {
                 lv_obj_create(NULL);
            }
            else {
                lv_obj_create(NULL);
            }
            break;
    }
}


void CreateMusicPlayer(void) {

    static lv_style_t bg_style;
    static lv_style_t bg_slider_style;
    static lv_style_t bg_imgbtn_style;
    static lv_style_t bg_label_style;

    lv_color_t bg_color;

    lv_style_init(&bg_style);
    bg_color = lv_color_make(0xbf , 0x36 , 0x0c);
    lv_style_set_bg_grad_color(&bg_style , bg_color);
    lv_style_set_bg_grad_dir(&bg_style , LV_GRAD_DIR_VER);

    lv_style_init(&bg_imgbtn_style);
    lv_style_set_img_recolor(&bg_imgbtn_style , lv_color_make(0x03 , 0x38 , 0x89));
    lv_style_set_img_recolor_opa(&bg_imgbtn_style,LV_OPA_30);

    /*      创建一个空scr       */
    MusicPlayerScr = lv_obj_create(NULL);
		lv_obj_clear_flag(MusicPlayerScr , LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(MusicPlayerScr,&bg_style , 0);
    //lv_scr_load(MusicPlayerScr); //仅仅创建 不载入


		/*      创建一个按钮      */
    lv_obj_t* btn = lv_btn_create(MusicPlayerScr);
    lv_obj_remove_style_all(btn);
    lv_obj_align_to(btn , MusicPlayerScr , LV_ALIGN_TOP_LEFT , 10 , 20);
    lv_obj_add_event_cb(btn , _userReturnCallBack , LV_EVENT_CLICKED , NULL);

    lv_obj_t *btntext = lv_label_create(btn);
    lv_obj_set_style_text_font(btntext , &ExternFont , 0);
    lv_obj_align(btntext , LV_ALIGN_CENTER ,0 , 0);
    lv_label_set_text(btntext , "<<");
    lv_obj_set_style_text_color(btntext , lv_color_make(0xff , 0x6e , 0x40) , 0);
		


     /*      添加音乐图片并使其旋转     */
    lv_obj_t *imgMusic = lv_img_create(MusicPlayerScr);
    lv_img_set_src(imgMusic , UserPicScrPath(music.png));
    lv_obj_align(imgMusic,LV_ALIGN_TOP_MID , 0 , lv_obj_get_height(MusicPlayerScr)/10);

    lv_timer_t *timer = lv_timer_create(_userTimerCallBack,100,(void *)imgMusic);
    lv_timer_ready(timer);
		
		

    /*      播放键     */ 
    lv_obj_t *imgbtnPlay = lv_imgbtn_create(MusicPlayerScr);
    lv_obj_set_size(imgbtnPlay , 64 , 64);
    lv_imgbtn_set_src(imgbtnPlay,LV_IMGBTN_STATE_RELEASED , NULL , UserPicScrPath(play.png) , NULL);
    lv_obj_align(imgbtnPlay,LV_ALIGN_BOTTOM_MID,0,-1 * (lv_obj_get_height(MusicPlayerScr)/10));
    lv_obj_add_event_cb(imgbtnPlay , _userPlayCallBack , LV_EVENT_CLICKED , timer); //定时器对象传递到回调
    lv_obj_add_style(imgbtnPlay , &bg_imgbtn_style , LV_STATE_PRESSED);


    /*      左切换键     */
    lv_obj_t *imgbtnLast = lv_imgbtn_create(MusicPlayerScr);
    lv_obj_set_size(imgbtnLast , 48 , 48);
    lv_imgbtn_set_src(imgbtnLast,LV_IMGBTN_STATE_RELEASED , NULL , UserPicScrPath(last.png) , NULL);
    lv_obj_align_to(imgbtnLast,imgbtnPlay,LV_ALIGN_LEFT_MID,-180,0);
    lv_obj_add_style(imgbtnLast , &bg_imgbtn_style , LV_STATE_PRESSED);


     /*      右切换键     */
    lv_obj_t *imgbtnNext = lv_imgbtn_create(MusicPlayerScr);
    lv_obj_set_size(imgbtnNext , 48 , 48);
    lv_imgbtn_set_src(imgbtnNext,LV_IMGBTN_STATE_RELEASED , NULL , UserPicScrPath(next.png) , NULL);
    lv_obj_align_to(imgbtnNext,imgbtnPlay,LV_ALIGN_LEFT_MID,200,0);
    lv_obj_add_style(imgbtnNext , &bg_imgbtn_style , LV_STATE_PRESSED);


    /*        滑块       */
    lv_style_init(&bg_slider_style);
    lv_style_set_bg_color(&bg_slider_style , lv_color_make(0xf5 , 0xf5 , 0xf5));
    lv_style_set_border_color(&bg_slider_style , lv_color_make(0xfa , 0xfa , 0xfa));

    lv_obj_t* slider = lv_slider_create(MusicPlayerScr);
    lv_obj_set_size(slider , lv_obj_get_width(MusicPlayerScr)* 0.5 , 3);
    lv_slider_set_range(slider , 0 , 100);
    lv_obj_add_flag(slider , LV_OBJ_FLAG_ADV_HITTEST); //使其只能拖动，不能点击
    lv_obj_set_ext_click_area(slider, 5);
    lv_obj_align_to(slider , imgbtnPlay,LV_ALIGN_OUT_TOP_MID,0,-50);
    lv_obj_add_event_cb(slider , _userSliderCallBack , LV_EVENT_VALUE_CHANGED , NULL);
    lv_obj_add_style(slider , &bg_slider_style , LV_PART_INDICATOR);
    lv_obj_add_style(slider , &bg_slider_style , LV_PART_KNOB);


    /*      添加label 显示歌词     */
    lv_style_init(&bg_label_style);
    lv_style_set_text_color(&bg_label_style , lv_color_make(0xf5 , 0xf5 , 0xf5));
    lv_style_set_text_font(&bg_label_style , &ExternFont);

    lv_obj_t* lyric = lv_label_create(MusicPlayerScr);
    lv_obj_set_size(lyric , lv_obj_get_width(MusicPlayerScr) , ExternFont.line_height);
    lv_obj_align_to(lyric , imgMusic , LV_ALIGN_OUT_BOTTOM_MID , 0 , 50);
    lv_label_set_text(lyric , "山明和水秀 不比你有看头");
    lv_obj_set_style_text_align(lyric ,LV_TEXT_ALIGN_CENTER , LV_STATE_DEFAULT);
    lv_obj_add_style(lyric , &bg_label_style , 0);
		
		     /*      添加label 显示总时长    */
    static lv_style_t time_style;
    lv_style_init(&time_style);
    lv_style_set_text_color(&time_style , lv_color_make(0xf5 , 0xf5 , 0xf5));
    lv_style_set_text_font(&time_style, &lv_font_montserrat_16);

    lv_obj_t *total_time = lv_label_create(MusicPlayerScr);
    lv_obj_set_size(total_time , 60 , lv_font_montserrat_16.line_height);
    lv_label_set_text(total_time , "04:30");
    lv_obj_set_style_text_align(total_time , LV_TEXT_ALIGN_CENTER , LV_STATE_DEFAULT);
    lv_obj_align_to(total_time , slider , LV_ALIGN_OUT_RIGHT_MID , 2 , 0);
    lv_obj_add_style(total_time , &time_style , 0);

     /*      添加label 显示当前时刻    */
    lv_obj_t *cur_time = lv_label_create(MusicPlayerScr);
    lv_obj_set_size(cur_time , 60 , lv_font_montserrat_16.line_height);
    lv_label_set_text(cur_time , "00:01");
    lv_obj_set_style_text_align(cur_time , LV_TEXT_ALIGN_CENTER , LV_STATE_DEFAULT);
    lv_obj_align_to(cur_time , slider , LV_ALIGN_OUT_LEFT_MID , -2 , 0);
    lv_obj_add_style(cur_time , &time_style , 0);
		
}













