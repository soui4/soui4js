import * as soui4 from "soui4";
import * as extctrl from "extctrl.dll";
import * as jsplayer from "jsvodplayer.dll";
import * as os from "os";
import * as std from "std";

class FilterInputDialog extends soui4.JsHostDialog{
	constructor(){
		super("layout:dlg_input_filter");
		this.onEvt = this.onEvent;
	}
	onEvent(e){
		 if(e.GetID()==8001)//event_exit
		{
			//get input data.
			let edit_input = this.FindIChildByName("edit_input");
			let str = new soui4.SStringA();
			edit_input.GetWindowText(str,false);
			this.inputStr=str.c_str();
			this.onEvt = 0;
		}
		return false;
	}
};

class SettingsDialog extends soui4.JsHostDialog{
	constructor(mainDlg){
		super("layout:dlg_settings");
		this.mainDlg = mainDlg;
		this.onEvt = this.onEvent;
		soui4.log("new SettingsDialog");
	}
	onEvent(e){
		if(e.GetID()==8000){//event_init
			let edit_video_path = this.FindIChildByName("edit_video_path");
			edit_video_path.SetWindowText(this.mainDlg.settings.video_path);
		}else if(e.GetID()==8001)//event_exit
		{
		}else if(e.GetID()==10000 && e.Sender().GetName()=="btn_pick_video_path"){
			this.onBtnPickVideoPath();
		}
	}

	onBtnPickVideoPath(){
		let videoPath = soui4.PickFolder(this.mainDlg.settings.video_path);
		if(videoPath != ""){
			let edit_video_path = this.FindIChildByName("edit_video_path");
			edit_video_path.SetWindowText(videoPath);
			this.mainDlg.settings.video_path = videoPath;
		}
	}
};

class AddRoomDialog extends soui4.JsHostDialog{
	constructor(platforms){
		super("layout:dlg_add_room");
		this.platforms = platforms;

		this.onEvt = this.onEvent;
	}

	onEvent(e){
		if(e.GetID()==8000){//event_init
			let cbx_platform = this.FindIChildByName("cbx_platform");
			let icbx_platform = soui4.QiIComboBox(cbx_platform);
			for(let i=0;i<this.platforms.length;i++){
				icbx_platform.InsertItem(i,this.platforms[i],0,i);
			}
			icbx_platform.Release();
		}else if(e.GetID()==8001)//event_exit
		{
			//get input data.
			let cbx_platform = this.FindIChildByName("cbx_platform");
			let str = new soui4.SStringA();
			cbx_platform.GetWindowText(str,false);
			this.platform = str.c_str();
			let edit_roomid = this.FindIChildByName("edit_room_id");
			edit_roomid.GetWindowText(str,false);
			this.room_id=str.c_str();
			let edit_roomdesc = this.FindIChildByName("edit_room_desc");
			edit_roomdesc.GetWindowText(str,false);
			this.room_desc=str.c_str();
			soui4.log("add room, platform="+this.platform+" room_id="+this.room_id+" room_desc="+this.room_desc);
			this.onEvt = 0;
		}
		return false;
	}
};

class RoomTvAdapter extends soui4.STvAdapter{
	constructor(mainDlg){
		super();
		this.mainDlg = mainDlg;
		this.roomList=[];
		this.favorRoot={platform:null,id:-1,desc:"我的收藏",url_path:null,item:0,"url":null,hot:1},
		this.onGetView = this.getView;
		this.isRefeshing = false;
		/*
		this.favorList =[
			{"platform":"YY","id":54880976,"desc":"yy/991","url_path":"data","item":0,"url":null},
			{"platform":"YY","id":22490906,"desc":"yy/993","url_path":"data","item":0,"url":null}
		];
		*/
		try{
			let f = std.open("favor.json", "r");
			let favorStr = f.readAsString();
			f.close();
			this.favorList = JSON.parse(favorStr);
			for(let i=0;i<this.favorList.length;i++){
				this.favorList[i].url=null;
				this.favorList[i].item=null;
			}
		}catch(e){
			this.favorList=[];
		}
	}

	getPlatformList(){
		let platforms=[];
		for(let i=0;i<this.roomList.length;i++){
			if(this.roomList[i].id==-2)
				platforms.push(this.roomList[i].platform);
		}
		return platforms;
	}

	addUserRoom(platform,id,desc){
		let roomInfo = null;
		for(let i=0;i<this.roomList.length;i++){
			if(this.roomList[i].platform==platform)
			{
				roomInfo = this.roomList[i];
				break;
			}
		}
		if(roomInfo == null)
			return false;
		let newRoom = {...roomInfo};
		newRoom.id = id;
		newRoom.desc = platform+"/"+desc;
		this.favorList.push(newRoom);
		let iRoom = this.roomList.length + 1 + this.favorList.length -1;
		let favorRoot = this.GetChildItem(0xffff0000,true);
		newRoom.item = this.InsertItem(iRoom,favorRoot,0xffff0002);
		this.notifyBranchChanged(favorRoot);
		this.checkRoom(iRoom);
		return true;
	}

	getRoomInfo(iRoom){
		if(iRoom<this.roomList.length)
		{
			return this.roomList[iRoom];
		}else if (iRoom==this.roomList.length){
			return this.favorRoot;
		}else
		{
			return this.favorList[iRoom-1-this.roomList.length];
		}
	}
	getView(/*HSTREEITEM*/ hItem, /*IWindow* */ pItem, /*IXmlNode* */pXmlTemplate){
		if(pItem.GetChildrenCount()==0){
			pItem.InitFromXml(pXmlTemplate);
		}
		let iRoom = this.GetItemData(hItem);
		//{platform:platformName,id:-1,desc:name,"url_path":url_path,"item":item,"url":null};//save item to roomList
		let roomInfo = this.getRoomInfo(iRoom);
		let txt = pItem.FindIChildByName("txt_label");		
		txt.SetWindowText(roomInfo.desc);
		soui4.SConnect(pItem,10000 + 11,this,this.onItemDbClick);
		soui4.SConnect(pItem,10000 + 8,this,this.onItemRClick);

		let img = pItem.FindIChildByName("img_state");
		let iImg = soui4.QiIImageWnd(img);
		if(roomInfo.id <0 )
		{
			let expland = this.IsItemExpended(hItem);
			iImg.SetIcon(expland?1:0);
		}else{
			if(roomInfo.url==null)
				iImg.SetIcon(2);//wait
			else if(roomInfo.url=="")
				iImg.SetIcon(3);//invalid
			else
				iImg.SetIcon(4);//ready.
		}
		
		let aniHot = pItem.FindIChildByName("ani_hot");
		aniHot.SetVisible(roomInfo.hot>0,true);
		iImg.Release();
	}

	onItemRClick(e){
		let pItem = soui4.toSWindow(e.Sender());
		let itemApi = soui4.QiIItemPanel(pItem);
		let favorRoot = this.GetChildItem(0xffff0000,true);
		let hItem = itemApi.GetItemIndex();
		let parentItem = this.GetParentItem(hItem);
		let iRoom = this.GetItemData(hItem);
		let roomInfo = this.getRoomInfo(iRoom);
		if(roomInfo.id >= 0){//rooInfo.id<0 is group or platform
			let e2 = soui4.toEventItemPanelRclick(e);
			let x = e2.lParam & 0xffff;
			let y = (e2.lParam>>16) & 0xffff;
			let pt = new soui4.CPoint(x,y);

			itemApi.PtToHost(pt);
			this.mainDlg.ClientToScreen(pt);

			let bAdd2Favor = parentItem!=favorRoot;
			let menu = new soui4.SMenu();
			menu.LoadMenu(bAdd2Favor?"smenu:favor_add":"smenu:favor_del");
			let cmd = menu.TrackPopupMenu(0x100,pt.x,pt.y,this.mainDlg.GetHwnd(),0,this.mainDlg.GetScale());
			if(bAdd2Favor && cmd == 1)
			{//add to favor
				let bExist = false;
				for(let i=0;i<this.favorList.length;i++){
					if(roomInfo.platform == this.favorList[i].platform &&
					roomInfo.id == this.favorList[i].id
					){
						bExist = true;
						break;
					}
				}
				if(bExist){
					soui4.SMessageBox(this.mainDlg.GetHwnd(),"该房间已经在收藏夹","提示",0);
				}else{
					let roomInfo2 = {...roomInfo};
					let iRoom = this.roomList.length + 1 + this.favorList.length;
					let newItem = this.InsertItem(iRoom,favorRoot,0xffff0002);
					roomInfo2.desc = roomInfo.platform+"/"+roomInfo.desc;
					roomInfo2.item = newItem;
					this.favorList[this.favorList.length] = roomInfo2;
					this.notifyBranchChanged(favorRoot);
				}
			}else if(!bAdd2Favor && cmd == 1){
				//remove from favor.
				this.DeleteItem(hItem,true);
				iRoom -= this.roomList.length +1; //get favor index.
				this.favorList.splice(iRoom,1); //remove this item
				//update all favor item index.
				iRoom = this.roomList.length +1;
				let favorItem = this.GetChildItem(favorRoot,true);
				while(favorItem!=0){
					this.SetItemData(favorItem,iRoom);
					iRoom ++;
					favorItem = this.GetNextSibling(favorItem);
				}
				//reflesh room treeview.
				this.notifyBranchChanged(favorRoot);
			}
		}
		itemApi.Release();
	}

	onItemDbClick(e){
		let pItem = soui4.toSWindow(e.Sender());
		let pItemPanel = soui4.QiIItemPanel(pItem);
		let hItem = pItemPanel.GetItemIndex();
		pItemPanel.Release();
		let iRoom = this.GetItemData(hItem);
		let roomInfo = this.getRoomInfo(iRoom);
		if(roomInfo.id<0){
			this.ExpandItem(hItem,3);//toggle state.
		}else if(roomInfo.url==null){
			soui4.SMessageBox(0,"wait for fetch live url,please wait a moment!","error",0);
		}else if(roomInfo.url != "")
		{
			//try to play url.
			this.mainDlg.playUrl(roomInfo.url);
		}
		return true;
	}

	initFavorList(){
		let iRoom = this.roomList.length;
		let itemFavorRoot= this.InsertItem(iRoom,0xffff0000,0xffff0001); //--0xffff0000 == STVI_ROOT, 0xffff0001==STVI_FIRST
		iRoom++;
		for(let i=0;i<this.favorList.length;i++){
			let itemFavor= this.InsertItem(iRoom+i,itemFavorRoot,0xffff0002);//favor id range from [-n,-1], -1 is favor root
			this.favorList[i].item = itemFavor;
		}
		soui4.log("initFavorList:"+itemFavorRoot);
		this.notifyBranchChanged(0xffff0000);//notify whole tree data changed.
		this.CheckRoomState(0);//check room state from 0 room
	}

	onRoomListResp(ctx,code,resp){
		let xml = new soui4.SXmlDoc();
		//let res = xml.LoadFile("d:\\roomlist.xml",116,0);//116=parse_default
		let res = xml.LoadString(resp,116);//116=parse_default
		this.roomList = []; //prepare a room list.
		if(res!=0){
			let xmlPlatform = xml.Root().FirstChild().FirstChild();
			let iRoom=0;
			while(!xmlPlatform.IsEmpty()){
				let platformName = xmlPlatform.Attribute("name",false).Value();
				let visible = xmlPlatform.Attribute("visible",false);
				if(visible!=0){
					visible=parseInt(visible.Value());
					if(visible==0){
						//hide this platform.
						xmlPlatform = xmlPlatform.NextSibling();
						continue;
					}
				}
				let url_path = xmlPlatform.Attribute("url_path",false);
				if(url_path!=0){
					url_path = url_path.Value();
				}else{
					url_path = "data";//read url from data element.
				}


				let itemPlatform= this.InsertItem(iRoom,0xffff0000,0xffff0002); //--0xffff0000 == STVI_ROOT, 0xffff0002==STVI_LAST
				this.roomList[iRoom]={platform:platformName,id:-2,desc:platformName,"url_path":url_path,"item":itemPlatform,"url":null,"hot":0};//save item to roomList
				iRoom++;
				let xmlType=xmlPlatform.FirstChild();
				
				while(!xmlType.IsEmpty()){
					let typeName=xmlType.FirstAttribute().Value();
					let itemType= this.InsertItem(iRoom,itemPlatform,0xffff0002); //--0xffff0000 == STVI_ROOT, 0xffff0002==STVI_LAST
					this.roomList[iRoom]={platform:platformName,id:-1,desc:typeName,"url_path":url_path,"item":itemType,"url":null,"hot":0};//save item to roomList
					iRoom++;

					let xmlRoom = xmlType.FirstChild();
					while(!xmlRoom.IsEmpty()){
						let roomId=xmlRoom.Attribute("id",false).Value();
						let roomDesc=xmlRoom.Attribute("desc",false).Value();
						let hotAttr=xmlRoom.Attribute("hot",false);
						let hot = 0;
						if(hotAttr!=0)
						{
							hot=parseInt(hotAttr.Value());
						}
						let item = this.InsertItem(iRoom,itemType,0xffff0002);
						this.roomList[iRoom]={platform:platformName,id:roomId,desc:roomDesc,"url_path":url_path,"item":item,"url":null,"hot":hot};//save item to roomList
						iRoom++;
						xmlRoom = xmlRoom.NextSibling();
					}
					
					xmlType = xmlType.NextSibling();
				}
				
				xmlPlatform = xmlPlatform.NextSibling();
			}
		}else{
			soui4.log("parse xml failed");
		}
		this.initFavorList();
	}
	onRoomListError(ctx,code){
		soui4.log("download room list failed, error code:"+code);
		this.initFavorList();
	}

	loadRoomList(){
		this.httpReq = new soui4.HttpRequest("https://soime.cn/soui_player/roomlist.xml","get");
		this.httpReq.cbHandler=this;
		this.httpReq.onResp=this.onRoomListResp;
		this.httpReq.onError=this.onRoomListError;
		this.httpReq.Execute();
	}

	getUrlFromResp(iRoom,resp){
		let ret=JSON.parse(resp);
		if(ret.state!=1 || ret.data=="null"){
			soui4.log("fetch url failed:"+resp);
			return null;
		}
		let roomInfo = this.getRoomInfo(iRoom);
		let url = null;
		if(roomInfo.url_path!="" && roomInfo.url_path!="data"){
			//parse room result for the platform.
			let path=roomInfo.url_path.split('/');
			
			for(let i=0;i<path.length;i++){
				let ipath = path[i];
				if(ipath.substr(0,1)=="."){
				    let idx = parseInt(ipath.substr(1,ipath.length-1));
					ret=ret[idx];
				}else{
					ret = ret[ipath];
				}					
			}
			url = ret;
		}else
		{
			url = ret.data;			
		}
		soui4.log("url:"+url);
		return url;
	}

	onCheckUrlResp(ctx,code,resp){
		let iRoom = ctx;
		soui4.log("request url succeed, iRoom="+iRoom+" code:"+code+" resp:"+resp);
		let url = this.getUrlFromResp(iRoom,resp);
		let roomInfo = this.getRoomInfo(iRoom);
		if(url!=null && url.substr(0,4)=="http"){
			roomInfo.url=url;
		}else{
			roomInfo.url="";//set url to empty.
		}
		this.notifyBranchInvalidated(roomInfo.item,false,false);
		this.CheckRoomState(ctx+1);
	}

	onCheckUrlErr(ctx,code){
		let iRoom = ctx;
		let roomInfo = this.getRoomInfo(iRoom);
		roomInfo.url="";
		this.notifyBranchInvalidated(roomInfo.item,false,false);
		soui4.log("request url failed, code:"+code+" iRoom="+iRoom);
		this.CheckRoomState(ctx+1);
	}

	checkRoom(iRoom){
		let roomInfo = this.getRoomInfo(iRoom);
		let url = "http://api.pyduck.com/live-api/get-url?live_platform="+roomInfo.platform+"&parameter="+roomInfo.id;
		soui4.log("checkRoom,url="+url);
		this.httpCheckUrl = new soui4.HttpRequest(url,"get");
		this.httpCheckUrl.cbHandler=this;
		this.httpCheckUrl.onResp = this.onCheckUrlResp;
		this.httpCheckUrl.onError = this.onCheckUrlErr;		
		this.httpCheckUrl.SetOpaque(iRoom);
		this.httpCheckUrl.Execute();
		this.mainDlg.OnCheckRoomProg(iRoom);
	}
	CheckRoomState(iRoom){
		if(iRoom == 0){
			this.mainDlg.OnCheckRoomStart(this.roomList.length + this.favorList.length +1);
			this.isRefeshing = true;
		}
		if(iRoom== this.roomList.length + this.favorList.length +1)
		{
			soui4.log("CheckRoomState finished!");
			this.mainDlg.OnCheckRoomStop();
			this.isRefeshing = false;
			return;
		}
		let roomInfo = this.getRoomInfo(iRoom);
		if(roomInfo.id<=0)
		{
			this.CheckRoomState(iRoom+1);
		}else{
			this.checkRoom(iRoom);
		}
	}

	refleshRoom(){
		if(this.isRefeshing)
			return;
		for(let i=0;i<this.roomList.length;i++){
			if(this.roomList[i].id>=0){
				this.roomList[i].url=null;
			}
		}
		for(let i=0;i<this.favorList.length;i++){
			this.favorList[i].url=null;
		}
		this.notifyBranchChanged(0xffff0000);
		this.CheckRoomState(0);
	}

	uninit(){
		//save favor list to db
		let f = std.open("favor.json", "w");
		let favorStr = JSON.stringify(this.favorList);
		f.puts(favorStr);
		f.close();

		this.mainDlg = null;
		if(this.httpReq!=undefined){
			this.httpReq.cbHandler = 0;
		}
		if(this.httpCheckUrl!=undefined){
			this.httpCheckUrl.cbHandler=0;
		}
	}
};

class CMainDlg extends soui4.JsHostDialog {
    constructor(resId) {
        super(resId);
		this.onMsg = this.onMessage;
		this.onEvt = this.onEvent;
		this.isLiveMode = false;
		this.vodPlayer = new jsplayer.SVodPlayer();
		this.vodPlayer.cbHandler = this;
		this.vodPlayer.onError = this.onError;
		this.vodPlayer.onStateChanged = this.onStateChanged;
		this.vodPlayer.onDuration=this.onDuration;
		this.vodPlayer.onPlayPosition = this.onPlayPosition;
		this.vodPlayer.onRecordStart = this.onRecordStart;
		this.vodPlayer.onRecordStop = this.onRecordStop;
		let sdlPresenter = jsplayer.CreateSdlPresenter(this);
		this.SetPresenter(sdlPresenter);
		this.vodPlayer.Init(sdlPresenter);
		this.vodPlayer.SetVolume(80);//init to 80%
		sdlPresenter.Release();
    }
    
	onError(errCode,statusCode){
		soui4.log("errCode:"+errCode+" statusCode:"+statusCode);
		this.onStop();
	}

	onStateChanged(state, reason){
		soui4.log("state:"+state+" reason:"+reason);
		this.state=state;
		if(state == 4){//playing
			soui4.log("start timer of 100ms");
			this.FindIChildByName("btn_resume").SetVisible(false,true);
			this.FindIChildByName("btn_pause").SetVisible(true,true);
		}else if(state==6){//stoped
			this.onStop();
		}else if(state == 5){//paused
			this.FindIChildByName("btn_resume").SetVisible(true,true);
			this.FindIChildByName("btn_pause").SetVisible(false,true);
		}
	}

	onPlayPosition(pos){
		let  pSlider = this.FindIChildByName("slider_prog");
		let sliderApi = soui4.QiIProgress(pSlider);
 		sliderApi.SetValue(pos);
		sliderApi.Release();
	}

	onDuration(totalTime){
		let  pSlider = this.FindIChildByName("slider_prog");
		let  pTxt = this.FindIChildByName("txt_duration");
		this.isLiveMode = totalTime==0x7fffffff;
		if(!this.isLiveMode)
		{
			let sliderApi = soui4.QiIProgress(pSlider);
			sliderApi.SetRange(0,totalTime);
			sliderApi.SetValue(0);
			sliderApi.Release();
			
			let ms = totalTime % 1000;
			totalTime = (totalTime-ms)/1000;
			let sec = totalTime % 60;
			totalTime = (totalTime-sec)/60;
			let minute = totalTime%60;
			totalTime = (totalTime-minute)/60;
			let hour = totalTime;
			
			pSlider.EnableWindow(true,true);
			if(hour>0)
				pTxt.SetWindowText(""+hour+":"+minute+":"+sec);
			else
				pTxt.SetWindowText(""+minute+":"+sec);	
			this.FindIChildByName("btn_pause").EnableWindow(true,true);
			this.FindIChildByName("btn_resume").EnableWindow(true,true);
		}else
		{
			pSlider.EnableWindow(false,true);
			pTxt.SetWindowText("--:--");
			this.FindIChildByName("btn_pause").EnableWindow(false,true);
			this.FindIChildByName("btn_resume").EnableWindow(false,true);
		}
	}

	onMessage(hwnd,msg,wp,lp,msgHandle){
		if(msg == 0x110){//WM_INITDIALOG==0x110
			this.onInit();
		}
		else if(msg==0x112 && wp==0xf060)//wm_syscommand and sc_close
		{
			this.onDestroy();
		}
		else{
			msgHandle.msgHandled =false;
		}
		return 0;
	}
	
	onEvent(e){
		let evtId=e.GetID();
		if(evtId == 10000 && e.Sender().GetName()=="btn_close"){
			this.onBtnClose();
			return true;
		}else if(evtId==8112 && e.Sender().GetName()=="curtain_left")//
		{
			soui4.log("on animation stop");
			if(this.playing){
				this.FindIChildByName("curtain_left").SetVisible(false,true);
				this.FindIChildByName("curtain_right").SetVisible(false,true);
			}
		}else if(evtId==17000 &&e.Sender().GetName()=="slider_prog"){
			let evtPos = soui4.toEventSliderPos(e);
			this.vodPlayer.Seek(evtPos.nPos);
		}else if(evtId==17000 &&e.Sender().GetName()=="slider_volume"){
			let evtPos = soui4.toEventSliderPos(e);
			this.vodPlayer.SetVolume(evtPos.nPos);
		}
		else if(evtId == 10000 && e.Sender().GetName()=="chk_enable_subtitle")
		{
			let subtitles = this.FindIChildByName("scroll_subtitles");
			let pChk = soui4.toSWindow(e.Sender());
			subtitles.SetVisible(pChk.IsChecked(),true);
		}else if(evtId == 15004 && e.Sender().GetName()=="room_tree"){
			//15004 == EVT_TC_DBCLICK
			let tcDbClick = soui4.toEventTCDbClick(e);
			let treeWnd = soui4.toSWindow(e.Sender());
			let treeCtrl = soui4.QiITreeCtrl(treeWnd);
			let iRoom = treeCtrl.GetItemData(tcDbClick.hItem);
			if(iRoom!=-1){
				this.requestRoomUrl(iRoom);
			}
		}else if(evtId==10000 &&e.Sender().GetName()=="btn_about"){
			this.onBtnAbout(e);
		}else if(evtId==10000 &&e.Sender().GetName()=="btn_settings"){
			this.onBtnSettings();
		}else if(evtId==9000 &&e.Sender().GetName()=="wnd_room_bookmark") //EVT_MOUSE_HOVER
		{
			if(this.hideRoomListTimer!=undefined)
			{
				os.clearTimeout(this.hideRoomListTimer);
				this.hideRoomListTimer=undefined;
			}
			if(!this.showRoom && this.showRoomListTimer==undefined)
				this.showRoomListTimer = os.setTimeout(function(){
					this.switchRoomList(true);
					this.showRoomListTimer=undefined;
				},50,this);
		}else if(evtId == 9001 && e.Sender().GetName()=="wnd_roomlist_container")//EVT_MOUSE_LEAVE
		{
			if(this.showRoomListTimer!=undefined)
			{
				os.clearTimeout(this.showRoomListTimer);
				this.showRoomListTimer=undefined;
			}
			if(this.showRoom && this.hideRoomListTimer==undefined)
				this.hideRoomListTimer = os.setTimeout(function(){
					this.switchRoomList(false);
					this.hideRoomListTimer = undefined;
				},50,this);			
		}else if(evtId==9000 &&e.Sender().GetName()=="wnd_root") //EVT_MOUSE_HOVER
		{
			this.switchCtrlPane(true);
		}else if(evtId == 9001 && e.Sender().GetName()=="wnd_root")//EVT_MOUSE_LEAVE
		{
			this.switchCtrlPane(false);
		}
		return false;
	}

	switchRoomList(bShow){
		this.showRoom = bShow;
		let leftPane = this.FindIChildByName("wnd_room_list");
		let theApp = soui4.GetApp();
		let anim = null;
		if(bShow) {
			anim = theApp.LoadAnimation("anim:slide_show_roomlist");
		}else{
			anim = theApp.LoadAnimation("anim:slide_hide_roomlist");
		}
		leftPane.SetAnimation(anim);
		anim.Release();
	}

	switchCtrlPane(bShow){
		if(!this.playing)
			return;
		let ctrlPane = this.FindIChildByName("pane_ctrl");
		let theApp = soui4.GetApp();
		let anim = null;
		if(bShow) {
			anim = theApp.LoadAnimation("anim:slide_show_ctrlpane");
		}else{
			anim = theApp.LoadAnimation("anim:slide_hide_ctrlpane");
		}
		ctrlPane.SetAnimation(anim);
		anim.Release();
	}

	onBtnClose(){
		soui4.log("on close click");
		this.DestroyWindow();
	}

	connect(btnName,evtId,func){
		let ctrl=this.FindIChildByName(btnName);
		if(ctrl == 0){
			soui4.log("find child by name "+btnName+ " failed");
			return;
		}
		let ret = soui4.SConnect(ctrl,evtId,this,func);
		soui4.log("connect ret "+ ret);
	}

	onDrop(fileCount){
		if(fileCount>=1){//recieve the first file.
			let buf = new soui4.SStringA();
			this.dropTarget.GetDropFileName(0,buf);
			let edit_url =this.FindIChildByName("edit_url");
			edit_url.SetWindowText(buf.c_str());
		}
	}

	onBtnAddRoom(e){
		let platforms=this.tvAdapter.getPlatformList();
		let dlg = new AddRoomDialog(platforms);
		let ret = dlg.DoModal(this.GetHwnd());
		if(ret == 1){//1==IDOK
			this.tvAdapter.addUserRoom(dlg.platform,dlg.room_id,dlg.room_desc);
		}
	}

	onSetFilter(){
		let dlg = new FilterInputDialog();
		if(dlg.DoModal(this.GetHwnd())==1){
			this.vodPlayer.SetVideoFilter(dlg.inputStr);
		}
		//this.vodPlayer.SetVideoFilter("scale=405:720,rotate=PI/2");		
		//this.vodPlayer.SetVideoFilter("rotate=PI/2");
	}
	onRecordStart(recordName,errCode){
		if(errCode==0){
			this.FindIChildByName("btn_record_start").SetVisible(false,false);
			this.FindIChildByName("btn_record_stop").SetVisible(true,true);
			let pApp = soui4.GetApp();
			let ani = pApp.LoadAnimation("anim:alpha_recording");
			let recording = this.FindIChildByName("indicator_recording");
			recording.SetAnimation(ani);
			ani.Release();
		}
	}
	onBtnStartRecord(e){
		let now = new Date();
		this.vodPlayer.StartRecord(this.settings.video_path+"\\record_"+now.getFullYear()+"_"+now.getMonth()+"_"+now.getDay()+"_"+now.getMinutes()+"_"+now.getSeconds()+".mp4");
	}
	onRecordStop(recordName,errCode){
		this.FindIChildByName("btn_record_start").SetVisible(true,false);
		this.FindIChildByName("btn_record_stop").SetVisible(false,true);
		let recording = this.FindIChildByName("indicator_recording");
		recording.ClearAnimation();
	}
	onBtnStopRecord(e){
		this.vodPlayer.StopRecord();
	}

	onBtnSettings(){
		soui4.log("onBtnSettings");
		let dlg = new SettingsDialog(this);
		dlg.DoModal(this.GetHwnd());
		//save to file.
		let f = std.open(g_WordDir+"\\settings.json", "w");
		let settingStr = JSON.stringify(this.settings);
		f.puts(settingStr);
		f.close();
	}

	onBtnPause(e){
		this.vodPlayer.Pause();
	}

	onBtnResume(e){
		this.vodPlayer.Resume();
	}

	onInit(){
		soui4.log("on init dialog");
		this.EnableDragDrop();

		try{
			let f = std.open(g_WordDir+"\\settings.json", "r");
			let settingStr = f.readAsString();
			f.close();
			this.settings = JSON.parse(settingStr);
		}catch(e){
			soui4.log("read settings.json failed!");
		}
		if(this.settings == undefined) this.settings={};
		if(this.settings.video_path == undefined){
			this.settings.video_path = soui4.getSpecialPath("video") + "\\sliveplayer";
			os.mkdir(this.settings.video_path);
		}
		//enable dropdrop.
		this.dropTarget = new soui4.SDropTarget();
		this.dropTarget.cbHandler = this;
		this.dropTarget.onDrop = this.onDrop;
		let dropAccept = this.FindIChildByName("sdl_back");		
		dropAccept.RegisterDragDrop(this.dropTarget);
		
		this.connect("btn_play",10000, this.onBtnPlay);
		this.connect("btn_stop",10000, this.onBtnStop);
		this.connect("btn_pause",10000, this.onBtnPause);
		this.connect("btn_resume",10000, this.onBtnResume);
		this.connect("btn_setFilter",10000, this.onSetFilter);
		this.connect("btn_reflesh_room",10000, this.onBtnRefleshRoom);
		this.connect("btn_add_room",10000,this.onBtnAddRoom);
		this.connect("btn_record_start",10000,this.onBtnStartRecord);
		this.connect("btn_record_stop",10000,this.onBtnStopRecord);
		//init room treeview.
		let room_tv = this.FindIChildByName("room_tv");
		let room_itv = soui4.QiITreeView(room_tv);
		this.tvAdapter = new RoomTvAdapter(this);
		room_itv.SetAdapter(this.tvAdapter);
		room_itv.Release();

		let wnd_check_room = this.FindIChildByName("prog_check_room");
		this.prog_check_room = soui4.QiIProgress(wnd_check_room);//save IProgress

		this.tvAdapter.loadRoomList();//load room list.
		this.showRoom = true;
	}
	
	onDestroy(){
		this.onStop();
		this.onEvt = 0;
		this.onMsg = 0;

		this.vodPlayer.cbHandler = 0;
		this.vodPlayer = null;
		this.prog_check_room.Release();
		this.prog_check_room=null;

		let dropAccept = this.FindIChildByName("sdl_back");		
		dropAccept.UnregisterDragDrop();
		this.dropTarget=null;

		let room_tv = this.FindIChildByName("room_tv");
		let room_itv = soui4.QiITreeView(room_tv);
		room_itv.SetAdapter(0);
		room_itv.Release();
		this.tvAdapter.uninit();
		this.tvAdapter=null;

		if(this.timer != undefined){
			os.clearTimeout(this.timer);
		}
		this.DestroyWindow();
	}

	onBtnStop(e){
		this.onStop();
	}
	onStop(){
		if(this.playing){
			this.vodPlayer.Stop();
			this.playing = false;
		}
		
		this.FindIChildByName("sdl_back").SetVisible(true,true);
		this.FindIChildByName("sdl_front").SetVisible(false,true);
		//关闭幕布动画
		{
			let pApp = soui4.GetApp();
			let pAniShowLeft = pApp.LoadAnimation("anim:slide_show_left");
			let curtain_left = this.FindIChildByName("curtain_left");
			curtain_left.SetVisible(true,true);
			curtain_left.SetAnimation(pAniShowLeft);
			pAniShowLeft.Release();
			let pAniShowRight = pApp.LoadAnimation("anim:slide_show_right");
			let curtain_right=this.FindIChildByName("curtain_right");
			curtain_right.SetVisible(true,true);
			curtain_right.SetAnimation(pAniShowRight);
			pAniShowRight.Release();
		}
	}
	onBtnPlay(e){
		let edit_url = this.FindIChildByName("edit_url");
		let str_url = new soui4.SStringA();
		edit_url.GetWindowText(str_url,false);
		let url = str_url.c_str();
		if(str_url.IsEmpty()){
			let filedlg = new soui4.SFileOpenDlg();
			filedlg.isSave=false;
			filedlg.defExt="mp4";
			filedlg.AddFilter("视频文件(*.mp4,*.mkv)","*.mp4;*.mkv");
			filedlg.AddFilter("所有文件(*.*)","*.*");
			let ret = filedlg.DoModal();
			if(ret != 1)
				return;
			url = filedlg.GetFilePath();
			edit_url.SetWindowText(url);
			
		}
		this.playUrl(url);
	}

	playUrl(url){
		this.FindIChildByName("sdl_back").SetVisible(false,false);
		this.FindIChildByName("sdl_front").SetVisible(true,true);
		//拉开幕布动画
		{
			let pApp = soui4.GetApp();
			let pAniHideLeft = pApp.LoadAnimation("anim:slide_hide_left");
			this.FindIChildByName("curtain_left").SetAnimation(pAniHideLeft);
			pAniHideLeft.Release();
			let pAniHideRight = pApp.LoadAnimation("anim:slide_hide_right");
			this.FindIChildByName("curtain_right").SetAnimation(pAniHideRight);
			pAniHideRight.Release();
		}

		soui4.log("url:"+url);
		this.vodPlayer.PlayUrl(url,100);
		this.playing=true;
	}

	onBtnAbout(e){
		let dialog = new soui4.JsHostDialog("layout:dlg_about");
		let ret = dialog.DoModal(this.GetHwnd());
	}

	onBtnRefleshRoom(){
		this.tvAdapter.refleshRoom();
	}

	OnCheckRoomStart(rooms){
		if(this.prog_check_room!=null){
			let progWnd = this.prog_check_room.ToIWindow();
			progWnd.SetVisible(true,true);
			this.prog_check_room.SetRange(0,rooms);
		}
	}

	OnCheckRoomProg(iRoom){
		if(this.prog_check_room!=null){
			this.prog_check_room.SetValue(iRoom);
		}
	}

	OnCheckRoomStop(){
		if(this.prog_check_room!=null){
			let progWnd = this.prog_check_room.ToIWindow();
			progWnd.SetVisible(false,true);
		}
	}
};

var g_WordDir;
function main(inst,workDir,args)
{
	soui4.log(workDir);
	g_WordDir = workDir;
	let theApp = soui4.GetApp();
	let souiFac = soui4.CreateSouiFactory();
	/*
	let resProvider = souiFac.CreateResProvider(1);
	soui4.InitFileResProvider(resProvider,workDir + "\\uires");
	//*/
	//*
	// show how to load resource from a zip file
	let resProvider = soui4.CreateZipResProvider(theApp,workDir +"\\uires.zip","souizip");
	if(resProvider === 0){
		soui4.log("load res from uires.zip failed");
		return -1;
	}
	//*/
	extctrl.RegisterCtrls(theApp);
	let resMgr = theApp.GetResProviderMgr();
	resMgr.AddResProvider(resProvider,"uidef:xml_init");
	resProvider.Release();
	let hwnd = soui4.GetActiveWindow();
	let hostWnd = new CMainDlg("xml\\dlg_main.xml");
	hostWnd.Create(hwnd,0,0,0,0);
	hostWnd.SendMessage(0x110,0,0);//send init dialog message.
	hostWnd.ShowWindow(1); //1==SW_SHOWNORMAL
	souiFac.Release();
	let ret= theApp.Run(hostWnd.GetHwnd());
	extctrl.UnregisterCtrls(theApp);
	soui4.log("js quit");
	return ret;
}

globalThis.main=main;