#include "gui.h"

using namespace ImGui;

bool ui::tab(int num) {
    // Validate tab index
    if (num < 0 || num >= static_cast<int>(tabs.size())) {
        return false;
    }

    // Validate icon and label
    auto icon = tabs[num].m_icon;
    auto label = tabs[num].m_label;
    if (!icon || !label) {
        return false;
    }

    // Validate window
    auto window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return false;
    }

    // Validate font
    auto font_ptr = font(1);
    if (!font_ptr) {
        return false;
    }

    auto style = ImGui::GetStyle();
    auto id = window->GetID(label);

    bool selected = num == next_tab;
    float selected_anim = ImTricks::Animations::FastFloatLerp(label, 0, selected, 0.f, 1.f, anim_speed);
    if (!std::isfinite(selected_anim)) {
        return false; // Invalid animation value
    }

    ImVec2 size(46 + text_size(1, icon).x + int((ImGui::CalcTextSize(label, 0, 1).x + 12) * selected_anim), window->Size.y);
    ImVec2 p = window->DC.CursorPos;
    ImRect bb(p, p + size);

    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) {
        return false;
    }

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    if (pressed && !selected) {
        content_anim_dest = 0.f;
        next_tab = num;
    }

    float anim = ImTricks::Animations::FastFloatLerp(label, 1, selected || hovered, 0.f, 1.f, anim_speed);
    if (!std::isfinite(anim)) {
        return false;
    }

    auto icon_col = ImTricks::Animations::FastColorLerp(ImGui::GetColorU32(ImGuiCol_TextDisabled), ImGui::GetColorU32(ImGuiCol_Scheme), anim);
    auto text_col = ImTricks::Animations::FastColorLerp(ImGui::GetColorU32(ImGuiCol_TextDisabled, selected_anim), ImGui::GetColorU32(ImGuiCol_Text, selected_anim), anim);

    window->DrawList->AddRectFilled(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg, selected_anim));
    window->DrawList->AddRectFilled({ bb.Min.x, bb.Max.y - 1 }, bb.Max, ImGui::GetColorU32(ImGuiCol_Scheme, selected_anim));

    window->DrawList->AddText(font_ptr, font_ptr->FontSize, { bb.Min.x + 23, bb.GetCenter().y - text_size(1, icon).y / 2 }, icon_col, icon);
    window->DrawList->AddText({ bb.Max.x - ImGui::CalcTextSize(label, 0, 1).x - 23, bb.GetCenter().y - ImGui::CalcTextSize(label, 0, 1).y / 2 }, text_col, label, ImGui::FindRenderedTextEnd(label));

    return pressed;
}

int rotation_start_index;
void ui::rotate_start( )
{
    rotation_start_index = GetWindowDrawList( )->VtxBuffer.Size;
}

ImVec2 ui::text_size( int n, const char* text ) {
    return font( n )->CalcTextSizeA( font( n )->FontSize, FLT_MAX, -1, text, FindRenderedTextEnd( text ) );
}

ImVec2 ui::rotation_center( )
{
    ImVec2 l( FLT_MAX, FLT_MAX ), u( -FLT_MAX, -FLT_MAX );

    const auto& buf = GetWindowDrawList( )->VtxBuffer;
    for ( int i = rotation_start_index; i < buf.Size; i++ )
        l = ImMin( l, buf[i].pos ), u = ImMax( u, buf[i].pos );

    return ImVec2( ( l.x + u.x ) / 2, ( l.y + u.y ) / 2 );
}

void ui::rotate_end( float rad, ImVec2 center )
{
    float s = sin( rad ), c = cos( rad );
    center = ImRotate( center, s, c) - center;

    auto& buf = GetWindowDrawList()->VtxBuffer;
    for ( int i = rotation_start_index; i < buf.Size; i++ )
        buf[i].pos = ImRotate( buf[i].pos, s, c ) - center;
}

void ui::handle_alpha_anim( ) {
    content_anim = ImLerp( content_anim, content_anim_dest, anim_speed * 2.5f );

    if ( content_anim < 0.001f ) {
        content_anim_dest = 1.f;
        cur_page = next_tab;
    }
}

void ui::render_page( ) {
    if ( tabs[cur_page].pages.size( ) == 0 || tabs[cur_page].pages.size( ) <= tabs[cur_page].cur_subtab )
        return;

    tabs[cur_page].pages[tabs[cur_page].cur_subtab]( );
}

void ui::add_page( int tab, std::function< void( ) > code ) {
    tabs[tab].pages.emplace_back( code );
}

void ui::begin_child( const char* name, ImVec2 size ) {
    BeginChild( std::string( name ).append( "main" ).c_str( ), CalcItemSize( size, GetWindowWidth( ) / 2 - GImGui->Style.ItemSpacing.x / 2 - 20, 0 ) );

    GetWindowDrawList( )->AddText( GetWindowPos( ) + ImVec2{ 24, 30 - CalcTextSize( name ).y / 2 }, GetColorU32( ImGuiCol_Text ), name, FindRenderedTextEnd( name ) );
    GetWindowDrawList( )->AddRectFilled( GetWindowPos( ) + ImVec2{ 24, 59 }, GetWindowPos( ) + ImVec2{ GetWindowSize( ).x, 60 }, GetColorU32( ImGuiCol_Border ) );

    SetCursorPosY( 60 );
    PushStyleVar( ImGuiStyleVar_WindowPadding, { 24, 24 } );
    BeginChild( name, { -1, size.y == 0 ? size.y : -1 }, 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysUseWindowPadding );
    PopStyleVar( );
    PushStyleVar( ImGuiStyleVar_ItemSpacing, { 18, 18 } );
}

void ui::end_child( ) {
    PopStyleVar( );
    EndChild( );
    EndChild( );
}

bool ui::color_btn( const char* str_id, float col[4], ImVec2 size ) {
    auto window = GetCurrentWindow( );
    auto style = GetStyle( );
    auto id = window->GetID( str_id );

    ImVec2 p = window->DC.CursorPos;
    ImRect bb( p, p + size );

    ItemSize( bb );
    ItemAdd( bb, id );

    bool hovered, held;
    bool pressed = ButtonBehavior( bb, id, &hovered, &held );

    window->DrawList->AddRectFilled( bb.Min, bb.Max, ImColor{ col[0], col[1], col[2], style.Alpha }, 2 );

    window->DrawList->AddRectFilled( { bb.GetCenter( ).x - bb.GetWidth( ) / 6, bb.Min.y }, { bb.GetCenter( ).x + bb.GetWidth( ) / 6, bb.GetCenter( ).y }, ImColor{ 1.f, 1.f, 1.f, GImGui->Style.Alpha * ( 1.f - col[3] ) * 0.6f } );
    window->DrawList->AddRectFilled( { bb.Min.x, bb.GetCenter( ).y }, { bb.Min.x + size.x / 3, bb.Max.y }, ImColor{ 1.f, 1.f, 1.f, GImGui->Style.Alpha * ( 1.f - col[3] ) * 0.6f }, 2, ImDrawFlags_RoundCornersBottomLeft );
    window->DrawList->AddRectFilled( { bb.Max.x - size.x / 3, bb.GetCenter( ).y }, bb.Max, ImColor{ 1.f, 1.f, 1.f, GImGui->Style.Alpha * ( 1.f - col[3] ) * 0.6f }, 2, ImDrawFlags_RoundCornersBottomRight );

    return pressed;
}

void ui::multi_select( const char* label, std::vector< multi_select_item >& items ) {
    auto window = GetCurrentWindow( );
    auto id = window->GetID( label );

    std::string buf;

    buf.clear( );
    for ( size_t i = 0; i < items.size( ); ++i ) {
        if ( items[i] ) {
            buf += items[i].label;
            buf += ", ";
        }
    }

    if ( !buf.empty( ) ) {
        buf.resize( buf.size( ) - 2 );
    }

    const ImGuiID popup_id = ImHashStr( "##ComboPopup", 0, id );
    bool popup_open = IsPopupOpen( popup_id, ImGuiPopupFlags_None );

    if ( popup_open )
        SetNextWindowSizeConstraints( ImVec2( 0, 0 ), ImVec2( FLT_MAX, 30 * items.size( ) + 20 ) );

    if ( !BeginCombo( label, buf.c_str( ) ) )
        return;

    SetCursorPos( { 0, 10 } );
    BeginGroup( );

    PushStyleVar( ImGuiStyleVar_ItemSpacing, { 0, 0 } );
    for ( int i = 0; i < items.size( ); ++i ) {
        if ( Selectable( items[i].label, items[i], ImGuiSelectableFlags_NoPadWithHalfSpacing | ImGuiSelectableFlags_DontClosePopups, { 0, 30 } ) )
            items[i].selected = !items[i].selected;
    }
    PopStyleVar( );

    EndGroup( );
    EndCombo( );
}

void ui::styles( ) {
    auto &style = GImGui->Style;

    style.WindowRounding = 0;
    style.WindowPadding = ImVec2{ 0, 0 };
    style.WindowBorderSize = 0;
    style.FrameRounding = 3;
    style.FramePadding = ImVec2{ 16, 14 };
    style.FrameBorderSize = 0;
    style.PopupRounding = 3;
    style.PopupBorderSize = 0;
    style.ChildRounding = 4;
    style.ChildBorderSize = 0;
    style.ItemSpacing = ImVec2{ 20, 20 };
    style.ItemInnerSpacing = ImVec2{ 10, 10 };
    style.ScrollbarRounding = 4;
    style.ScrollbarSize = 4;
}
void ui::colors() {
    ImVec4* colors = GImGui->Style.Colors;

    // Keepin' the OG gray vibes for bgs, but buttons gettin' that purple Skibidi drip 💜
    colors[ImGuiCol_Scheme] = ImColor(138, 43, 226); // Purple scheme hittin' like a TikTok banger
    colors[ImGuiCol_Text] = ImColor(188, 188, 188); // Gray text, no Ohio fumbles
    colors[ImGuiCol_TextDisabled] = ImColor(94, 95, 98);   // Disabled text stayin' lowkey gray
    colors[ImGuiCol_WindowBg] = ImColor(27, 28, 28);   // OG dark gray window bg, no changes
    colors[ImGuiCol_ChildBg] = ImColor(33, 34, 36);   // Child bg keepin' it gray, sigma energy
    colors[ImGuiCol_FrameBg] = ImColor(40, 41, 43);   // Frame bg stayin' gray, unbothered
    colors[ImGuiCol_PopupBg] = colors[ImGuiCol_FrameBg]; // Popups matchin' gray frame, clean W

    // Borders gettin' a lil purple aura, no Ohio ratio
    colors[ImGuiCol_Border] = ImColor(150, 100, 200, 50); // Semi-transparent purple, maximum Rizz
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // No shadow, we stayin' drippy

    // Scrollbar poppin' off with purple GYATT
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.0f); // Keepin' it dark, no Ohio Ls
    colors[ImGuiCol_ScrollbarGrab] = colors[ImGuiCol_Scheme]; // Purple grab got that Gen Z energy
    colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(160, 60, 255); // Hover purple so bright it’s moggin'
    colors[ImGuiCol_ScrollbarGrabActive] = ImColor(180, 80, 255); // Active purple hittin' different fr

    // Checkmarks and sliders gettin' that purple glow, no Ohio vibes
    colors[ImGuiCol_CheckMark] = ImColor(200, 150, 255); // Light purple checkmark, pure aura
    colors[ImGuiCol_SliderGrab] = ImColor(200, 150, 255); // Slider grab drippin' in lavender

    // Buttons so purple they eatin' good, straight W
    colors[ImGuiCol_Button] = ImColor(138, 43, 226);  // Base purple button, Gen Alpha approved
    colors[ImGuiCol_ButtonHovered] = ImColor(160, 60, 255);  // Hover purple got that extra Rizzler
    colors[ImGuiCol_ButtonActive] = ImColor(180, 80, 255);  // Active purple, we stayin' unbothered

    // Separator and selection keepin' it purple and drippy
    colors[ImGuiCol_Separator] = colors[ImGuiCol_Border]; // Purple border vibes, no Ohio energy
    colors[ImGuiCol_TextSelectedBg] = ImColor(138, 43, 226, 50); // Purple selection, lowkey aura
}