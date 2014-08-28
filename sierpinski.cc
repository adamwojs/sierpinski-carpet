//Program rysujący dywan Sierpińskiego
//Kompilacja: g++ `pkg-config gtkmm-2.4 --cflags --libs` nazwa.cpp

#include <gtkmm.h>
#include <math.h>

//Maksymalny krok algorytmu...
const int max_step = 6;

struct Sqr {
	int a;
	int x; 
	int y;
};

class AppWindow : public Gtk::Window {
	public:
		AppWindow();
		virtual ~AppWindow();

		//Klasa powierzchni po której rysujemy...
		class CarpetArea : public Gtk::DrawingArea {
			public:
				CarpetArea() { 
					n = 1;
					set_size_request(759, 756);
				};
				virtual ~CarpetArea() {};

				void draw_carpet(int n); //Rysuje dywan o n krokach
				
			protected:
				virtual bool on_expose_event(GdkEventExpose *event);
			
			private:
				int n; //Krok...
				
				Cairo::RefPtr<Cairo::Context> carpet; //Powierzchnia dywanu
				
				void draw_step(struct Sqr &rect, int n); //Metoda z głównym algorytmem
				//Ustawia kwadrat, wzasadzie można by było dorzucić do struct Sqr...
				void set_rect(struct Sqr &rect, int x, int y, int a) {
					rect.x = x;
					rect.y = y;
					rect.a = a;
				}
		};
	
	protected:
		Gtk::HScale 	step_scale;
		CarpetArea 	carpet_area;
		
		virtual void on_step_scale_changed(); 			//Gdy zmieniono skale...
			
};

AppWindow::AppWindow() {
	//Ustawiania skali:
	step_scale.set_draw_value(false);
	step_scale.set_digits(0);
	step_scale.set_range(1, max_step);
	step_scale.set_increments(1, 1);

	//Sygnały...
	step_scale.signal_value_changed().connect(
		sigc::mem_fun(*this, &AppWindow::on_step_scale_changed)
	);

	//Dodajemy widżety do okna...
	Gtk::VBox  *main_conteiner = Gtk::manage(new Gtk::VBox); 
	Gtk::HBox *scale_conteiner = Gtk::manage(new Gtk::HBox);
	scale_conteiner->set_spacing(10);
	Gtk::Label *scale_desc = Gtk::manage(new Gtk::Label("Krok n:", Gtk::ALIGN_LEFT));
	scale_conteiner->pack_start(*scale_desc, Gtk::PACK_SHRINK);	
	scale_conteiner->pack_start(step_scale);	
	main_conteiner->pack_start(*scale_conteiner, Gtk::PACK_SHRINK);	
	main_conteiner->pack_start(carpet_area);
	add(*main_conteiner);
	
	show_all_children();	
}

AppWindow::~AppWindow() {}

void AppWindow::on_step_scale_changed() {
	carpet_area.draw_carpet((int) step_scale.get_value());
}

bool AppWindow::CarpetArea::on_expose_event(GdkEventExpose *event) {
	carpet = get_window()->create_cairo_context();
	
	//Czyścimy widok
	carpet->set_source_rgb(1, 1, 1);
	carpet->rectangle(0, 0, get_allocation().get_width(), get_allocation().get_height());
	carpet->paint();
	
	//Gwóźdź programu: Rysujemy dywan!	
	//Ustawienia rysowania:
	carpet->set_line_width(1);
	carpet->set_antialias(Cairo::ANTIALIAS_NONE);
	carpet->set_source_rgb(0, 0, 0); //Kolor kwadratów dywanu...
	
	//Rozpoczynamy rysowanie !
	struct Sqr rect;
	set_rect(rect, 0, 0, get_allocation().get_width()); //Obszar jest i tak kwadratowy
	
	draw_step(rect, n); 
	
	return true;
}

void AppWindow::CarpetArea::draw_carpet(int _n) {
	//Incjajacja zmiennych globalnych dla algorytmu 
	n = _n;
	
	//Odświeżamy dywan uwzględniając nową wartość...
	if (Glib::RefPtr<Gdk::Window> wnd = get_window()) {
		Gdk::Rectangle wnd_rect(
			0, 0, get_allocation().get_width(), get_allocation().get_height() 
		);
		wnd->invalidate_rect(wnd_rect, false);
	}
}

void AppWindow::CarpetArea::draw_step(struct Sqr &rect, int n) {
	if (n <= 0) return ;
	
	int a = rect.a / 3; //Długość boku kwadratu składowego
	
	//Rysujemy kwadrat śrotkowy...	
	carpet->move_to(a + rect.x-2, a + rect.y-2);
	carpet->rel_line_to(0, a);
	carpet->rel_line_to(a, 0);
	carpet->rel_line_to(0, -a);
	carpet->rel_line_to(-a, 0);
	carpet->stroke();
	
	//Rysujemy subkwadraty...
	//Pierwszy wiersz:
	struct Sqr child_rect;
	
	child_rect = rect;
	set_rect(child_rect, 	0 + rect.x, 	0 + rect.y, 	a);
	draw_step(child_rect, n-1); 
	child_rect = rect;
	set_rect(child_rect, 	a + rect.x, 	0 + rect.y, 	a);
	draw_step(child_rect, n-1);
	
	child_rect = rect;
	set_rect(child_rect, 	a+a + rect.x, 	0 + rect.y, 	a);
	draw_step(child_rect, n-1);
	
	//Drugi wiersz:
	child_rect = rect;
	set_rect(child_rect,	0 + rect.x,	a + rect.y,	a);
	draw_step(child_rect, n-1);
	
	//draw_step(); //Środek pozostawiamy pusty...
	
	child_rect = rect;
	set_rect(child_rect,	a+a + rect.x, 	a + rect.y,	a);
	draw_step(child_rect, n-1);
			
	//Trzecie wiersz:
	child_rect = rect;
	set_rect(child_rect,	0 + rect.x,	a+a + rect.y,	a);
	draw_step(child_rect, n-1);
	
	child_rect = rect;
	set_rect(child_rect,	a + rect.x,	a+a + rect.y,	a);
	draw_step(child_rect, n-1);
	
	child_rect = rect;
	set_rect(child_rect,	a+a + rect.x,	a+a + rect.y,	a);
	draw_step(child_rect, n-1);	
}

int main(int argc, char *argv[]) {
	Gtk::Main kit(argc, argv);
		
	AppWindow main_wnd;
	//Ustawienia okna:
	main_wnd.set_title("Dywan Sierpińskiego (write by SCT/Adawo), License: GPL");
	main_wnd.set_resizable(false);
	main_wnd.set_border_width(10);
	
	Gtk::Main::run(main_wnd);	
	
	return 0;
}
