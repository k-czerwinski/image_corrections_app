#include "GUIMyFrame1.h"


GUIMyFrame1::GUIMyFrame1(wxWindow* parent) : MyFrame1(parent) {
	//controls will be enabled when we load image
	disableButtons();

}

void GUIMyFrame1::Load_File_ButtonOnButtonClick(wxCommandEvent& event) {
	wxFileDialog openFileDialog(this, "Choose a file", "", "", "Image files (*.png;*.jpg)|*.png;*.jpg", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	wxImage::AddHandler(new wxJPEGHandler);
	wxImage::AddHandler(new wxPNGHandler);
	wxImage image;
	{
		wxLogNull logNo;
		if (!image.LoadFile(openFileDialog.GetPath())) {
			wxMessageBox(_("Sprobuj ponownie zaladowac obrazek"));
			return;
		}
		orgImage = image.Copy();
	}
	processingFullSizeImage = orgImage.Copy();
	prepareScaledThumbnail();
	afterScroll();
	displayMainImage();
	enableButtons();
}

void GUIMyFrame1::SaveFileButtonOnButtonClick(wxCommandEvent& event) {
	wxFileDialog saveFileDialog(this, "Choose a file", "", "", "Image Files (*.png;*.jpg)|*.pnh;*.jpg", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (saveFileDialog.ShowModal() == wxID_CANCEL) return;
	wxImage saveImage = processingFullSizeImage.Copy();
	saveImage.AddHandler(new wxJPEGHandler);
	saveImage.AddHandler(new wxPNGHandler);
	saveImage.SaveFile(saveFileDialog.GetPath());
}


void GUIMyFrame1::Horizontal_ScrollbarOnScroll(wxScrollEvent& event) {
	afterScroll();
	displayMainImage();
}

void GUIMyFrame1::Vertical_ScrollbarOnScroll(wxScrollEvent& event) {
	afterScroll();
	displayMainImage();
}

void GUIMyFrame1::Brightness_SliderOnScroll(wxScrollEvent& event) {
	Brightness(Brightness_Slider->GetValue());
	displayMainImage();
}

void GUIMyFrame1::Contrast_SliderOnScroll(wxScrollEvent& event) {
	Contrast(Contrast_Slider->GetValue());
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
	currentOnScreenImageOrg = currentOnScreenImage.Copy();

	//reseting sliders positions
	Brightness_Slider->SetValue(0);
	Contrast_Slider->SetValue(0);
	Gamma_Slider->SetValue(0);

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

void GUIMyFrame1::Brightness(int value) {
	currentOnScreenImage = currentOnScreenImageOrg.Copy();
	unsigned char* temp = currentOnScreenImage.GetData();
	unsigned char* temp1 = (unsigned char*)malloc(currentOnScreenImage.GetWidth() *currentOnScreenImage.GetHeight() * 3);
	int x;
	for (int i = 0; i < currentOnScreenImage.GetWidth() * currentOnScreenImage.GetHeight() * 3; i++) {

		x = temp[i] + value;

		if (x > 255)	temp1[i] = 255;
		else if (x < 0)	temp1[i] = 0;
		else temp1[i] = x;

	}
	currentOnScreenImage.SetData(temp1);
}

void GUIMyFrame1::Contrast(int value) {
	currentOnScreenImage = currentOnScreenImageOrg.Copy();
	unsigned char* temp = currentOnScreenImage.GetData();
	unsigned char* temp1 = (unsigned char*)malloc(currentOnScreenImage.GetWidth() * currentOnScreenImage.GetHeight() * 3);
	int x;

	double newwal;

	if (value < 0) {
		newwal = (value + 100) / 100.0;
	}
	else if (value > 0) {
		newwal = 1 + value / 10.0;
	}
	else {
		newwal = 1;
	}


	for (int i = 0; i < currentOnScreenImage.GetWidth() * currentOnScreenImage.GetHeight() * 3; i++) {

		x = (temp[i] - 128) * newwal + 128;

		if (x > 255)	temp1[i] = 255;
		else if (x < 0)	temp1[i] = 0;
		else temp1[i] = x;

	}
	currentOnScreenImage.SetData(temp1);
}