#include "GUIMyFrame1.h"

GUIMyFrame1::GUIMyFrame1(wxWindow* parent) : MyFrame1(parent) {
	//controls will be enabled when we load image
	disableButtons();

	wxImage::AddHandler(new wxJPEGHandler);
	wxImage::AddHandler(new wxPNGHandler);
}

void GUIMyFrame1::Load_File_ButtonOnButtonClick(wxCommandEvent& event) {
	wxFileDialog openFileDialog(this, "Choose a file", "", "", "JPG Files (*.jpg)|*.jpg| PNG Files (*.png)|*png", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	//wxImage image;
	if (!orgImage.LoadFile(openFileDialog.GetPath())) {
		wxMessageBox(_("Sprobuj ponownie zaladowac obrazek"));
		return;
	}
	
	processingFullSizeImage = orgImage.Copy();
	prepareScaledThumbnail();
	afterScroll();
	displayMainImage();
	enableButtons();
}

void GUIMyFrame1::Horizontal_ScrollbarOnScroll(wxScrollEvent& event) {
	afterScroll();
	displayMainImage();
}
void GUIMyFrame1::Vertical_ScrollbarOnScroll(wxScrollEvent& event) {
	afterScroll();
	displayMainImage();
}

void GUIMyFrame1::disableButtons() {
	Histogram_Button->Disable();
	SaveFileButton->Disable();
	Red_Checkbox->Disable();
	Green_Checkbox->Disable();
	Blue_Checkbox->Disable();
	Brightness_Slider->Disable();
	Contrast_Slider->Disable();
	Gamma_Slider->Disable();
	Apply_Color_Button->Disable();
	Dark_Checkbox->Disable();
	Medium_CheckBox->Disable();
	Bright_Checkbox->Disable();
	Apply_Size_Button->Disable();
	Horizontal_Scrollbar->Disable();
	Vertical_Scrollbar->Disable();
	Reset_Button->Disable();
}

void GUIMyFrame1::enableButtons() {
	Histogram_Button->Enable();
	SaveFileButton->Enable();
	Red_Checkbox->Enable();
	Green_Checkbox->Enable();
	Blue_Checkbox->Enable();
	Brightness_Slider->Enable();
	Contrast_Slider->Enable();
	Gamma_Slider->Enable();
	Apply_Color_Button->Enable();
	Dark_Checkbox->Enable();
	Medium_CheckBox->Enable();
	Bright_Checkbox->Enable();
	Apply_Size_Button->Enable();
	Horizontal_Scrollbar->Enable();
	Vertical_Scrollbar->Enable();
	Reset_Button->Enable();
}

//This method rescale miniature panel and display rescaled miniature photo
void GUIMyFrame1::displayThumbnail(int xRectPos, int yRectPos) {
	if (!photoThumbnail.IsOk())
		return;
	wxBitmap bitmap(photoThumbnail);
	wxClientDC dc(Miniature_Panel);
	dc.Clear();
	dc.DrawBitmap(bitmap, 0, 0, true);

	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(wxPen(wxColour(255,0,0),5));
	dc.DrawRectangle(xRectPos,yRectPos, selectedRectSize.x, selectedRectSize.y);

}

void GUIMyFrame1::prepareScaledThumbnail() {
	double propotion = static_cast<double>(orgImage.GetWidth()) / orgImage.GetHeight();
	int panelWidth = Miniature_Panel->GetSize().x;
	wxSize miniatureSize = wxSize(panelWidth, panelWidth/propotion);
	Miniature_Panel->SetSize(miniatureSize);
	photoThumbnail = orgImage.Scale(miniatureSize.x, miniatureSize.y);

	//adjusting scrollbars to new panel size
	Horizontal_Scrollbar->SetSize(wxSize(miniatureSize.x, 20));
	Vertical_Scrollbar->SetSize(wxSize(20, miniatureSize.y));
	selectedRectSize = wxSize(miniatureSize.x / miniatureSizeToRectSize, miniatureSize.y / miniatureSizeToRectSize);
}


void GUIMyFrame1::afterScroll() {
	int xPos = static_cast<double>(Horizontal_Scrollbar->GetThumbPosition()) / Horizontal_Scrollbar->GetRange() * Miniature_Panel->GetSize().x * (1 - 1.0 / miniatureSizeToRectSize);
	int yPos = static_cast<double>(Vertical_Scrollbar->GetThumbPosition()) / Vertical_Scrollbar->GetRange() * Miniature_Panel->GetSize().y * (1 - 1.0 / miniatureSizeToRectSize);
	displayThumbnail(xPos, yPos);
	int orgXPos = static_cast<double>(xPos) / Miniature_Panel->GetSize().x * orgImage.GetWidth();
	int orgYPos = static_cast<double>(yPos) / Miniature_Panel->GetSize().y * orgImage.GetHeight();
	//preparing image on main screen
	currentOnScreenImage = processingFullSizeImage.GetSubImage(wxRect(orgXPos, orgYPos, orgImage.GetWidth() / miniatureSizeToRectSize, orgImage.GetHeight() / miniatureSizeToRectSize));
	/*
		COLOR ADJUSMENT WILL BE ADDED THERE
	*/
	
}

void GUIMyFrame1::displayMainImage() {
	if (!currentOnScreenImage.IsOk())
		return;
	wxBitmap bitmap(currentOnScreenImage);
	wxClientDC dc(Main_Panel);
	dc.DrawBitmap(bitmap, 0, 0, true);
}