#include "C:/Wichtig/System/Static/Library/WindowEngine1.0.h"

Sprite sprite;

long long FirstPress = 0;
long long LastTime = 0;
short KeyBefore = 0;
short CharBefore = 0;
int Curser = 0;
int CurserEnd = 0;

int OffsetX = 200;
int OffsetY = 200;
int ScrollX = 0;
int ScrollY = 0;

int Size = 64;
int PathSize = 32;

char FontPath[] = "C:/Wichtig/System/Fonts/Alx_Font_Blocky.png";

Font OutPath;
String* Buffer = NULL;
String Text;
String SavingPath;

float Curser_X(String* s,int Curser){
    float CurserX = 0;
    for(int i = Curser-1;i>=0;i--){
        if(String_Get(s,i)=='\n' || i==0){
            CurserX = Curser - i - 1;
            if(i==0) CurserX++;
            break;
        }
    }

    CurserX += (float)OffsetX / (float)Size;
    if(Buffer==&Text)   CurserX += ScrollX;
    else                CurserX += 1;

    return CurserX;
}
float Curser_Y(String* s,int Curser){
    float Lines = 0;
    for(int i = 0;i<Curser;i++){
        if(String_Get(s,i)=='\n'){
            Lines++;
        }
    }

    if(Buffer==&Text){
        Lines += (float)OffsetY / (float)Size;
        Lines += ScrollY;
    }

    return Lines;
}
int Curser_Up(String* s,int Curser){
    int CurserX = 0;
    for(int i = Curser;i>=0;i--){
        if(String_Get(s,i)=='\n'){
            CurserX = Curser - i;
            break;
        }
    }
    int TopX = 0;
    for(int i = Curser-CurserX-1;i>=0;i--){
        if(String_Get(s,i)=='\n' || i==0){
            TopX = (Curser - CurserX) - i;
            if(CurserX<TopX)    Curser = i + CurserX;
            else                Curser = i + TopX;
            if(i==0) Curser--;
            break;
        }
    }
    return Curser;
}
int Curser_Down(String* s,int Curser){
    int CurserX = 0;
    for(int i = Curser-1;i>=0;i--){
        if(String_Get(s,i)=='\n' || i==0){
            CurserX = Curser - i;
            if(i==0) CurserX++;
            break;
        }
    }
    if(Curser==0) CurserX++;
    
    int BottomX = 0;
    for(int i = Curser;i<=s->str.size;i++){
        if(String_Get(s,i)=='\n' || i==s->str.size){
            BottomX = i;
            break;
        }
    }

    int TopX = 0;
    for(int i = BottomX+1;i<s->str.size;i++){
        if(String_Get(s,i)=='\n' || i==s->str.size-1){
            if(i-BottomX<CurserX)       Curser = i + 1;
            else                        Curser = BottomX + CurserX;
            if(Curser==s->str.size-1)   Curser = i;
            break;
        }
    }
    return Curser;
}
int Curser_Get(String* s,int x,int y){
    x -= OffsetX;
    if(Buffer==&Text)   y -= OffsetY;

    if(Buffer==&Text)   x /= Size;
    else                x /= PathSize;
    if(Buffer==&Text)   y /= Size;
    else                y /= PathSize;

    if(Buffer==&Text)   x -= ScrollX;
    if(Buffer==&Text)   y -= ScrollY;
    
    int Lines = 0;
    int Chars = 0;
    for(int i = 0;i<s->str.size;i++){
        if(String_Get(s,i)=='\n'){
            if(Lines==y && Chars<=x){
                return i;
            }
            Lines++;
            Chars = 0;
        }
        if(Lines==y && Chars==x){
            if(Lines==0) i--;
            return i+1;
        }
        Chars++;
    }
    return s->str.size;
}

void Setup(AlxWindow* w){
    //sprite = Sprite_Load("C:/Wichtig/System/Fonts/Alx_Font_Blocky.png");
    SetFont(Sprite_Load(FontPath),16,16,16,16);
    ResizeFont(Size,Size);

    OutPath = Font_New(Sprite_Load(FontPath),16,16,16,16);
    Font_Resize(&OutPath,PathSize,PathSize);

    Text = String_Make("");
    SavingPath = String_Make("C:/Wichtig/Hecke/C/Win_Editor/Saved.c");

    Buffer = &Text;

    Curser = -1;
    CurserEnd = -1;
}

void Update(AlxWindow* w){

    char Again = Time_Nano()-LastTime>50000000;
    if(FirstPress==0){
        FirstPress = Time_Nano();
        Again = 1;
    }else if(Time_Nano()-FirstPress<500000000){
        Again = 0;
    }
    if(window.LastKey==0){
        FirstPress = 0;
    }

    if(Again){
        CharBefore = 0;
        KeyBefore = 0;
        LastTime = Time_Nano();
    }

    if(Again && Stroke(VK_OEM_PLUS).DOWN && Stroke(VK_CONTROL).DOWN){
        Size += 10;
        SetFont(Sprite_Load(FontPath),16,16,16,16);
        ResizeFont(Size,Size);
    }else if(Again && Stroke(VK_OEM_MINUS).DOWN && Stroke(VK_CONTROL).DOWN){
        Size -= 10;
        SetFont(Sprite_Load(FontPath),16,16,16,16);
        ResizeFont(Size,Size);
    }else if(Again && Stroke('C').DOWN && Stroke(VK_CONTROL).DOWN){
        if(CurserEnd>=0){
            int Up = Curser<CurserEnd?Curser:CurserEnd;
            int Down = Curser>CurserEnd?Curser:CurserEnd;

            char* CStr = String_SubCStr(Buffer,Up,Down-Up);
            int Size = CStr_Size(CStr)+1;

            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE,Size);
            memcpy(GlobalLock(hMem),CStr,Size);
            GlobalUnlock(hMem);
            OpenClipboard(0);
            EmptyClipboard();
            SetClipboardData(CF_TEXT,hMem);
            CloseClipboard();

            free(CStr);
            CurserEnd = -1;
        }
    }else if(Again && Stroke('V').DOWN && Stroke(VK_CONTROL).DOWN){
        if(OpenClipboard(NULL)){
            HANDLE hData = GetClipboardData(CF_TEXT);
            if(hData!=NULL){
                char* CStr = (char*)GlobalLock(hData);
                if(CStr!=NULL){
                    String_Add(Buffer,CStr,Curser);
                }
                GlobalUnlock(hData);
            }
            CloseClipboard();
        }
        CurserEnd = -1;
    }else if(Again && Stroke('X').DOWN && Stroke(VK_CONTROL).DOWN){
        if(CurserEnd>=0){
            int Up = Curser<CurserEnd?Curser:CurserEnd;
            int Down = Curser>CurserEnd?Curser:CurserEnd;

            char* CStr = String_SubCStr(Buffer,Up,Down-Up);
            int Size = CStr_Size(CStr)+1;

            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE,Size);
            memcpy(GlobalLock(hMem),CStr,Size);
            GlobalUnlock(hMem);
            OpenClipboard(0);
            EmptyClipboard();
            SetClipboardData(CF_TEXT,hMem);
            CloseClipboard();

            for(int i = Down;i>=Up;i--){
                String_Remove(Buffer,i);
            }

            free(CStr);
            CurserEnd = -1;
        }
    }else if(window.LastChar!=CharBefore && (window.LastChar>=32 && window.LastChar<127)){
        if(CurserEnd>=0){
            if(Curser!=CurserEnd){
                int Up = Curser<CurserEnd?Curser:CurserEnd;
                int Down = Curser>CurserEnd?Curser:CurserEnd;

                for(int i = Down;i>=Up;i--){
                    String_Remove(Buffer,i);
                }
                Curser = Up;
            }
        }else{
            String_AddChar(Buffer,window.LastChar,Curser);
        
            Curser++;
            CharBefore = window.LastChar;
        }
        CurserEnd = -1;
    }else if(Again && Stroke(VK_BACK).DOWN){
        if(CurserEnd>=0){
            int Up = Curser<CurserEnd?Curser:CurserEnd;
            int Down = Curser>CurserEnd?Curser:CurserEnd;

            for(int i = Down;i>=Up;i--){
                String_Remove(Buffer,i);
            }
            Curser = Up;
        }else{
            Curser = (int)F32_Clamp((float)Curser,(float)0,(float)Buffer->str.size);
            Curser--;
            if(Curser>=0)   String_Remove(Buffer,Curser);
            else            Curser = 0;
        }
        CurserEnd = -1;
    }else if(Again && Stroke(VK_RETURN).DOWN){
        String_AddChar(Buffer,'\n',Curser);
        Curser++;
        CharBefore = '\n';
        CurserEnd = -1;
    }else if(window.LastKey!=KeyBefore && Stroke(VK_LEFT).DOWN){
        Curser--;
        KeyBefore = window.LastKey;
        if(!Stroke(VK_LSHIFT).DOWN) CurserEnd = -1;
    }else if(window.LastKey!=KeyBefore && Stroke(VK_RIGHT).DOWN){
        Curser++;
        KeyBefore = window.LastKey;
        if(!Stroke(VK_LSHIFT).DOWN) CurserEnd = -1;
    }else if(window.LastKey!=KeyBefore && Stroke(VK_UP).DOWN){
        Curser = Curser_Up(Buffer,Curser);
        KeyBefore = window.LastKey;
        if(!Stroke(VK_LSHIFT).DOWN) CurserEnd = -1;
    }else if(window.LastKey!=KeyBefore && Stroke(VK_DOWN).DOWN){
        Curser = Curser_Down(Buffer,Curser);
        KeyBefore = window.LastKey;
        if(!Stroke(VK_LSHIFT).DOWN) CurserEnd = -1;
    }
    
    if(Stroke(VK_LBUTTON).PRESSED){
        Curser = Curser_Get(Buffer,GetMouse().x,GetMouse().y);
    }
    if(Stroke(VK_LBUTTON).DOWN){
        CurserEnd = Curser_Get(Buffer,GetMouse().x,GetMouse().y);
    }
    Curser = (int)F32_Clamp((float)Curser,(float)0,(float)Buffer->str.size);

    Clear(BLACK);

    RenderRect(0.0f,0.0f,GetWidth(),OffsetY,GRAY);
    RenderRect(0.0f,0.0f,OffsetX,GetHeight(),GRAY);

    if(Stroke(VK_LBUTTON).PRESSED && GetMouse().x>OffsetX && GetMouse().y<OffsetY){
        Buffer = &SavingPath;
    }else if(Stroke(VK_LBUTTON).PRESSED && GetMouse().x>OffsetX && GetMouse().y>OffsetY){
        Buffer = &Text;
    }else if(Stroke(VK_LBUTTON).PRESSED && GetMouse().x<OffsetX && GetMouse().y<OffsetY/2){
        char* Path = String_CStr(&SavingPath);
        char* Data = String_CStr(&Text);
        
        FILE* f = fopen(Path,"w");
        if(f){
            fprintf(f,"%s",Data);
            fclose(f);
        }

        free(Path);
        free(Data);
    }else if(Stroke(VK_LBUTTON).PRESSED && GetMouse().x<OffsetX && GetMouse().y<OffsetY && GetMouse().y>OffsetY/2){
        char* Path = String_CStr(&SavingPath);
        
        FILE* f = fopen(Path,"r");
        if(f){
            fseek(f,0,SEEK_END);
            int Length = ftell(f);
            fseek(f,0,SEEK_SET);

            char* BufferIn = (char*)malloc(Length+1);
            fread(BufferIn,1,Length,f);
            BufferIn[Length] = '\0';

            String_Clear(&Text);
            String_Append(&Text,BufferIn);
            free(BufferIn);

            fclose(f);
        }

        free(Path);
    }

    RenderRect(0.0f,0.0f,OffsetX,OffsetY/2,RED);
    RenderCStrFont(&OutPath,"SAVE",0,0,BLACK);
    RenderRect(0.0f,OffsetY/2,OffsetX,OffsetY/2,GREEN);
    RenderCStrFont(&OutPath,"LOAD",0,OffsetY/2,BLACK);

    char* cstr = String_CStr(&SavingPath);
    RenderCStrFont(&OutPath,cstr,OffsetX,0,BLACK);
    free(cstr);

    //RenderSpriteAlphaTint(&sprite,10.0f,10.0f,RED);
    //RenderSpriteAlphaTint(&sprite,GetMouse().x,GetMouse().y,RED);
    //RenderSubSprite(&sprite,GetMouse().x,GetMouse().y,32.0f,32.0f,16.0f,16.0f);
    //RenderSubSpriteAlphaTint(&sprite,GetMouse().x,GetMouse().y,32.0f,64.0f,16.0f,16.0f,BLACK);

    //RenderSpriteResize(&sprite,GetMouse().x,GetMouse().y,1000,500);
    //char* cstr = String_CStr(Buffer);
    //RenderCStr(cstr,0.0f,0.0f,RED);
    //RenderChar('_',Curser * GetFont()->CharSizeX,0.0f,WHITE);
    //free(cstr);

    //RenderSpriteAlphaTint(&GetFont()->Atlas,GetMouse().x,GetMouse().y,BLUE);

    int Chop = 0;
    int Lines = 0;
    int Chars = 0;

    int CharsCurser = 0;
    int LinesCurser = 0;

    for(int i = Chop;i<Text.str.size;i++){
        char c = String_Get(&Text,i);
        if(c=='\n' || i==Text.str.size-1){
            char Buff[20];
            sprintf(Buff,"%d",Lines);
            RenderCStr(Buff,10,OffsetY + Lines * GetFont()->CharSizeY,BLACK);

            char* cstr = String_SubCStr(&Text,Chop,i-Chop+1);
            RenderCStr(cstr,OffsetX,OffsetY + Lines * GetFont()->CharSizeY,WHITE);
            free(cstr);

            Chars = i - Chop;
            if(Curser>=Chop && Curser<=i+1){
                CharsCurser = Curser-Chop;
                LinesCurser = Lines;
            }
            Chop = i+1;
            Lines++;
        }
        if(c=='\n' && i==Text.str.size-1){
            CharsCurser = 0;
            LinesCurser = Lines;
            Lines++;
            Chars = -1;
        }
    }

    int UsedSize = 0;
    if(Buffer==&Text)   UsedSize = Size;
    else                UsedSize = PathSize;
    RenderChar('_',Curser_X(Buffer,Curser) * UsedSize,Curser_Y(Buffer,Curser) * UsedSize,RED);

    if(Curser!=CurserEnd && CurserEnd>=0){
        int Up = Curser<CurserEnd?Curser:CurserEnd;
        int Down = Curser>CurserEnd?Curser:CurserEnd;

        int Before = Up;
        for(int i = Up;;i++){
            if(String_Get(Buffer,i)=='\n'){
                int X = (i+1<Down?i+1:Down) - Before;
                RenderRectAlpha(Curser_X(Buffer,Before) * UsedSize,(Curser_Y(Buffer,Before)) * UsedSize,X * UsedSize,UsedSize,0x990000FF);
                Before = i+1;
            }
            if(i>=Down && String_Get(Buffer,i)=='\n'){
                break;
            }
        }
    }
}

void Delete(AlxWindow* w){
    //Sprite_Delete(&sprite);

    Font_Free(&OutPath);

    String_Free(&Text);
    String_Free(&SavingPath);
}

int main(){
    if(Create("Game Test",1900,1000,1,1,Setup,Update,Delete))
        Start();
    return 0;
}