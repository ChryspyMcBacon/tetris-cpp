#include "tetris.hpp"

static bool power_of_2(uint32_t n){
    if (n == 0) return false;
    while (n > 1) {
        if (n % 2 != 0)	
        	return false;
        n = n / 2;
    }
    return true;
}

piece::piece() : m_side(0), m_color(0), m_grid(nullptr){}

piece::piece(uint32_t s, uint8_t c) : m_side(s), m_color(c), m_grid(nullptr){
	if(!power_of_2(s))	throw tetris_exception("Side length must be a power of 2");
	
	if(c == 0)	throw tetris_exception("Il colore non può essere 0");

	m_grid = new bool*[m_side];
	for(uint32_t row = 0; row < m_side; row++){
		m_grid[row] = new bool[m_side];
		for(uint32_t col = 0; col < m_side; col++){
			m_grid[row][col] = false;	 				//inizializzo tutto a vuoto
		}
	}
}

piece::piece(piece const& rhs) : m_side(rhs.m_side), m_color(rhs.m_color), m_grid(nullptr){
	if(m_side > 0){
		m_grid = new bool*[m_side];
		for(uint32_t row = 0; row < m_side; row++){
			m_grid[row] = new bool[m_side];
			for(uint32_t col = 0; col < m_side; col++){
				m_grid[row][col] = rhs.m_grid[row][col];	
			}
		}	
	}
}

piece::piece(piece&& rhs) : m_side(rhs.m_side), m_color(rhs.m_color), m_grid(rhs.m_grid){  //rubbo la memoria
    rhs.m_side = 0;
    rhs.m_color = 0;
    rhs.m_grid = nullptr;
}

piece::~piece(){
	if(m_grid){
		for(uint32_t i = 0; i < m_side; i++){
			delete[] m_grid[i];
		}
		delete[] m_grid;
	}
}

piece& piece::operator=(piece const& rhs){
	if(this != &rhs){
		if(m_grid){
			for(uint32_t i = 0; i < m_side; i++){
			delete[] m_grid[i];
			}
		delete[] m_grid;
		}

		m_side = rhs.m_side;
		m_color = rhs.m_color;
		if(m_side > 0){
			m_grid = new bool*[m_side];
			for(uint32_t row = 0; row < m_side; row++){
				m_grid[row] = new bool[m_side];
				for(uint32_t col = 0; col < m_side; col++){
					m_grid[row][col] = rhs.m_grid[row][col];
				}
			}	
		}
	}

	return *this;
}

piece& piece::operator=(piece&& rhs){
    if (this != &rhs) {
        if (m_grid) {
            for (uint32_t i = 0; i < m_side; i++) {
                delete[] m_grid[i];
            }
            delete[] m_grid;
        }
        
        m_side = rhs.m_side;
        m_color = rhs.m_color;
        m_grid = rhs.m_grid;
        
        rhs.m_side = 0;
        rhs.m_color = 0;
        rhs.m_grid = nullptr;
    }
    return *this;
}

uint32_t piece::side() const{
	return m_side;
}

int piece::color() const{
	return m_color;
}

bool piece::operator()(uint32_t i, uint32_t j) const{
    if (i >= m_side || j >= m_side)    throw tetris_exception("Indice fuori dai bordi");
    
    return m_grid[i][j];
}

bool& piece::operator()(uint32_t i, uint32_t j){
    if (i >= m_side || j >= m_side)    throw tetris_exception("Indice fuori dai bordi");
    
    return m_grid[i][j];
}

bool piece::empty(uint32_t i, uint32_t j, uint32_t s) const{
	if(i + s > m_side || j + s > m_side)	throw tetris_exception("Indice fuori dai bordi");

	for(uint32_t row = i; row < i + s; row++){
		for(uint32_t col = j; col < j + s; col++){
			if(m_grid[row][col])
				return false;
		}
	}
	return true;
}

bool piece::full(uint32_t i, uint32_t j, uint32_t s) const{
	if(i + s > m_side || j + s > m_side)	throw tetris_exception("Indice fuori dai bordi");
	
	for(uint32_t row = i; row < i + s; row++){
		for(uint32_t col = j; col < j + s; col++){
			if(!m_grid[row][col])
				return false;
		}
	}
	return true;
}

bool piece::empty() const{
	if(m_side == 0) return true;

	for(uint32_t row = 0; row < m_side; row++){
		for(uint32_t col = 0; col < m_side; col++){
			if(m_grid[row][col])
				return false;
		}
	}
	return true;
}

bool piece::full() const{
	if(m_side == 0) return false;

	for(uint32_t row = 0; row < m_side; row++){
		for(uint32_t col = 0; col < m_side; col++){
			if(!m_grid[row][col])
				return false;
		}
	}
	return true;
}

void piece::rotate(){
    if (m_side <= 1) return; 

    bool** temp = new bool*[m_side];
    for(uint32_t i = 0; i < m_side; i++){
    	temp[i] = new bool[m_side];
    }

    for (uint32_t row = 0; row < m_side; row++) {
        for (uint32_t col = 0; col < m_side; col++) {
        	temp[col][m_side - row - 1] = m_grid[row][col];  //90° orario
        }
    }

    for(uint32_t i = 0; i < m_side; i++){
    	delete[] m_grid[i];
    }
    delete[] m_grid;

    m_grid = temp;
}

void piece::cut_row(uint32_t i){
	if(i >= m_side)	throw tetris_exception("Indice fuori dai bordi");
	
	for(uint32_t row = i; row > 0; row--){ 		 	//shifto le righe verso il basso
		for(uint32_t col = 0; col < m_side; col++)
			m_grid[row][col] = m_grid[row - 1][col];
	}

	for(uint32_t col = 0; col < m_side; col++){
		m_grid[0][col] = false;  					//riga superiore vuota
	}
}

bool piece::operator==(piece const& rhs) const{
	if(m_side != rhs.m_side || m_color != rhs.m_color)	return false;

	for(uint32_t row = 0; row < m_side; row++){
		for(uint32_t col = 0; col < m_side; col++){
			if(m_grid[row][col] != rhs.m_grid[row][col])
				return false;
		}
	}
	return true;
}

bool piece::operator!=(piece const& rhs) const{
    return !(*this == rhs);
}

static void skip_whitespace(std::istream& is){
    while (is.peek() == ' ' || is.peek() == '\t' || is.peek() == '\n'){
        is.get();
    }
}

static void parse_recursive(std::istream& is, piece& p, uint32_t side, uint32_t i, uint32_t j){
	skip_whitespace(is);
	if(!is.good())	throw tetris_exception("Fine prematura dell'input");
	
	char c;
	if(!(is >> c))	throw tetris_exception("Impossibile leggere carattere");

	if(c == '('){
		skip_whitespace(is);
		if(!is.good())	throw tetris_exception("Fine prematura dell'input dopo '('");
         
		char next_c = is.peek();

		if(next_c == ')'){  	//quadrante pieno - ()
			is >> c;
			for(uint32_t row = i; row < i + side; row++){
				for (uint32_t col = j; col < j + side; col++){
					p(row,col) = true;
				}
			}
		}else if(next_c == '(' || next_c == '['){  	//divisione in 4 sotto-quadranti
			if(side < 2)	throw tetris_exception("Impossibile dividere ulteriormente: side troppo piccolo");
			
			uint32_t half = side / 2;
			parse_recursive(is, p, half, i, j); 		  	 	//TL
			parse_recursive(is, p, half, i, j + half); 		 	//TR
			parse_recursive(is, p, half, i + half, j); 		 	//BL
			parse_recursive(is, p, half, i + half, j + half); 	//BR

			skip_whitespace(is); 
			if(!(is >> c) || c != ')')	throw tetris_exception("Parentesi tonda non chiusa correttamente");
			
		}else{
			throw tetris_exception("Carattere invalido dopo '(': atteso ')', '(' o '['");
		}
	}else if(c == '['){  		// quadrante vuoto - []
		skip_whitespace(is);
		if(!(is >> c) || c != ']')	throw tetris_exception("Parentesi quadra non chiusa");
	}else{
		throw tetris_exception("Carattere invalido atteso '(' o '['");	
	}
}

std::istream& operator>>(std::istream& is, piece& p){
    uint32_t side;
    uint16_t tempColor;

    skip_whitespace(is);
    if(!(is >> side))      throw tetris_exception("Impossibile leggere la dimensione");
    if(!power_of_2(side))  throw tetris_exception("La dimensione deve essere una potenza di 2");

    skip_whitespace(is);
    if(!(is >> tempColor)) throw tetris_exception("Impossibile leggere il colore");
    if(tempColor < 1 || tempColor > 255) throw tetris_exception("Colore fuori range (1-255)");

    piece temp(side, static_cast<uint8_t>(tempColor));
    parse_recursive(is, temp, side, 0, 0);

    p = std::move(temp);
    return is;
}

static void write_recursive(std::ostream& os, piece const& p,  uint32_t side, uint32_t i, uint32_t j){
	if(p.empty(i, j, side))     os << "[]";    //quadrante vuoto
    else if(p.full(i, j, side)) os << "()";    //quadrante pieno
	else{
		os << "(";

		uint32_t half = side / 2;
		write_recursive(os, p, half, i, j);					//TL
		write_recursive(os, p, half, i, j + half);			//TR
		write_recursive(os, p, half, i + half, j);			//BL
		write_recursive(os, p, half, i + half, j + half);	//BR

		os << ")";
	} 
}

std::ostream& operator<<(std::ostream& os, piece const& p){
    os << p.side() << " " << (int)p.color() << " ";
    write_recursive(os, p, p.side(), 0, 0);

    return os;
}

tetris::tetris() : m_score(0), m_width(0), m_height(0), m_field(nullptr){}

tetris::tetris(uint32_t w, uint32_t h, uint32_t s): m_score(s), m_width(w), m_height(h), m_field(nullptr){
	if (w == 0 || h == 0)	throw tetris_exception("Lunghezza e altezza devono essere maggiori di 0");
}

tetris::tetris(tetris const& rhs) : m_score(rhs.m_score), m_width(rhs.m_width), m_height(rhs.m_height), m_field(nullptr){
	if(rhs.m_field){
		m_field = new node{rhs.m_field->tp, nullptr};
		node* current = m_field;
		node* rhs_current = rhs.m_field->next;

		while(rhs_current){
			current->next = new node{rhs_current->tp, nullptr};

			current = current->next;
			rhs_current = rhs_current->next;
		}
	}
}

tetris::tetris(tetris&& rhs): m_score(rhs.m_score), m_width(rhs.m_width), m_height(rhs.m_height), m_field(rhs.m_field){
    rhs.m_score = 0;
    rhs.m_width = 0;
    rhs.m_height = 0;
    rhs.m_field = nullptr;
}

tetris::~tetris(){
	while(m_field){
		node* temp = m_field;
		m_field = m_field->next;
		delete temp;
	}
}

tetris& tetris::operator=(tetris const& rhs){
	if(this != &rhs){							//controllo auto-assegnamento
		while(m_field){							//libero la memoria esistente
			node* temp = m_field;
			m_field = m_field->next;
			delete temp;
		}

		m_score = rhs.m_score;
		m_width = rhs.m_width;
		m_height = rhs.m_height;
		m_field = nullptr;

		if(rhs.m_field){									//se rhs.m_field esiste allora copio
			m_field = new node{rhs.m_field->tp, nullptr};
            node* current = m_field;
            node* rhs_current = rhs.m_field->next;
            
            while(rhs_current != nullptr){
                current->next = new node{rhs_current->tp, nullptr};
                current = current->next;
                rhs_current = rhs_current->next;
            }
		}
	}
	return *this;
}

tetris& tetris::operator=(tetris&& rhs){
    if(this != &rhs){					//controllo auto-assegnamento
        while(m_field != nullptr){		//libero la memoria esistente
            node* temp = m_field;
            m_field = m_field->next;
            delete temp;
        }
        
        m_score = rhs.m_score;				//rubbo la memoria
        m_width = rhs.m_width;
        m_height = rhs.m_height;
        m_field = rhs.m_field;
        
        rhs.m_score = 0;  					//invalido rhs
        rhs.m_width = 0;
        rhs.m_height = 0;
        rhs.m_field = nullptr;
    }
    return *this;
}

bool tetris::operator==(tetris const& rhs) const{
	if(m_score != rhs.m_score || m_height != rhs.m_height || m_width != rhs.m_width){
		return false;
	}

	node* current = m_field;
	node* rhs_current = rhs.m_field;

	while(current && rhs_current){
		if(current->tp.x != rhs_current->tp.x || current->tp.y != rhs_current->tp.y || current->tp.p != rhs_current->tp.p)
			return false;

		current = current->next;
		rhs_current = rhs_current->next;
	}

	return (!current && !rhs_current);
}

bool tetris::operator!=(tetris const& rhs) const{
    return !(*this == rhs);
}


bool tetris::containment(piece const& p, int x, int y) const{
    bool collision = false;
    uint32_t r = 0;
    
    while(r < p.side() && !collision){
        uint32_t c = 0;
        while(c < p.side() && !collision){
            if(p(r, c)){				//se la cella del pezzo è piena
                int fx = x + c;
                int fy = y - ((int)p.side() - 1 - r);		

                if(fx < 0 || fx >= (int)m_width || fy < 0 || fy >= (int)m_height){	// controllo i bordi
                    collision = true;
                }else{
                    node* current = m_field;			          // Check collisioni lista
                    while(current && !collision){
                        int ox = current->tp.x;
                        int oy = current->tp.y;
                        uint32_t oside = current->tp.p.side();

                        int ocol = fx - ox;
                        int orow = fy - oy + (int)oside - 1;

                        if(ocol >= 0 && ocol < (int)oside && orow >= 0 && orow < (int)oside){
                            if(current->tp.p(orow, ocol)){
                                collision = true;			// collisione trovata
                            }
                        }
                        current = current->next;
                    }
                }
            }
            c++;
        }
        r++;
    }
    return !collision;
}

void tetris::add(piece const& p, int x, int y){
    if(!containment(p, x, y))    throw tetris_exception("Impossibile aggiungere il pezzo");
    
    m_field = new node{{p, x, y}, m_field};		//inserisco in testa
}

void tetris::insert(piece const& p, int x){
    int best_y = -1;				 		//cerco la y più grande (più in basso)
    bool found_valid_position = false;
    
    int cy = 0;							//trovo la posizione più bassa
    int limit = m_height + p.side();
    
    bool searching = true;
    while(cy < limit && searching){
        if(containment(p, x, cy)){
            best_y = cy;
            found_valid_position = true;	//questa y è valida, potrebbe esserci di meglio
        }else{
            if(found_valid_position){
                searching = false;			//ho già trovato una y valida e ora mi fermo
            }
        }
        if(searching){
             cy++;
        }
    }

    if(!found_valid_position){
        throw tetris_exception("GAME OVER");
    }

    add(p, x, best_y);

    bool changed = true;
    while(changed){
        changed = false;
        uint32_t* row_counts = new uint32_t[m_height]();	
        
        node* curr = m_field;
        while(curr) {								//conto le celle piene riga per riga su tutti i pezzi				
            uint32_t s = curr->tp.p.side();				
            for(uint32_t row = 0; row < s; row++){
                for(uint32_t col = 0; col < s; col++){
                    if(curr->tp.p(row,col)){
                        int fy = curr->tp.y - (s - 1 - row);
                        if(fy >= 0 && fy < (int)m_height){
                            row_counts[fy]++;
                        }
                    }
                }
            }
            curr = curr->next;
        }

        for(uint32_t i = 0; i < m_height; ++i){
            if(row_counts[i] == m_width){			//riga piena, taglio
                m_score += m_width;
                node* r_curr = m_field;
                while(r_curr){
                    int side = r_curr->tp.p.side();
                    int pr = side - 1 - (r_curr->tp.y - (int)i);
                    if(pr >= 0 && pr < side){
                        r_curr->tp.p.cut_row(pr);
                    }
                    r_curr = r_curr->next;
                }
                changed = true;
            }
        }
        delete[] row_counts;

        node** link = &m_field;
        while(*link){
            if((*link)->tp.p.empty()){			//controllo se ho dei pezzi rimasti vuoti dopo i tagli
                node* dead = *link;
                *link = (*link)->next;
                delete dead;
                changed = true;
            } else {
                link = &((*link)->next);
            }
        }

        curr = m_field;
        while(curr){						//provo a far scendere ogni pezzo di una riga
            int current_y = curr->tp.y;
            int target_y = current_y + 1;
            
            bool can_move = true;
            uint32_t s = curr->tp.p.side();
            
            uint32_t r = 0;
            while (r < s && can_move) {
                uint32_t c = 0;
                while(c < s && can_move){
                    if(curr->tp.p(r,c)){
                        int fx = curr->tp.x + c;
                        int fy = target_y - (s - 1 - r);

                        if(fy >= (int)m_height){
                            can_move = false;
                        }else{
                            node* other = m_field;
                            while(other && can_move){
                                if(other != curr){ 				//ovviamente non controllo il pezzo con se stesso
                                    int ox = other->tp.x;
                                    int oy = other->tp.y;
                                    uint32_t os = other->tp.p.side();
                                    
                                    int col_in_o = fx - ox;
                                    int row_in_o = fy - oy + (int)os - 1;

                                    if(col_in_o >= 0 && col_in_o < (int)os && row_in_o >= 0 && row_in_o < (int)os){
                                        if(other->tp.p(row_in_o, col_in_o)){
                                            can_move = false;					//collisione
                                        }
                                    }
                                }
                                if(can_move) other = other->next;
                            }
                        }
                    }
                    if(can_move) c++;
                }
                if(can_move) r++;
            }

            if(can_move){
                curr->tp.y = target_y;		//il pezzo scende
                changed = true;
            }

            curr = curr->next;
        }
    }
}

uint32_t tetris::score() const{
	return m_score;
}

uint32_t tetris::width() const{
	return m_width; 
}

uint32_t tetris::height() const{
	return m_height; 
}

tetris::iterator::iterator(node* ptr) : m_ptr(ptr){}

tetris_piece& tetris::iterator::operator*(){
    return m_ptr->tp;
}

tetris_piece* tetris::iterator::operator->(){
    return &(m_ptr->tp);
}

tetris::iterator& tetris::iterator::operator++(){
    m_ptr = m_ptr->next;
    return *this;
}

tetris::iterator tetris::iterator::operator++(int){
    iterator temp = *this;
    m_ptr = m_ptr->next;
    return temp;
}

bool tetris::iterator::operator==(iterator const& rhs) const{
    return m_ptr == rhs.m_ptr;
}

bool tetris::iterator::operator!=(iterator const& rhs) const{
    return m_ptr != rhs.m_ptr;
}

tetris::const_iterator::const_iterator(node const* ptr) : m_ptr(ptr){}

tetris_piece const& tetris::const_iterator::operator*() const{
    return m_ptr->tp;
}

tetris_piece const* tetris::const_iterator::operator->() const{
    return &(m_ptr->tp);
}

tetris::const_iterator& tetris::const_iterator::operator++(){
    m_ptr = m_ptr->next;
    return *this;
}

tetris::const_iterator tetris::const_iterator::operator++(int){
    const_iterator temp = *this;
    m_ptr = m_ptr->next;
    return temp;
}

bool tetris::const_iterator::operator==(const_iterator const& rhs) const{
    return m_ptr == rhs.m_ptr;
}

bool tetris::const_iterator::operator!=(const_iterator const& rhs) const{
    return m_ptr != rhs.m_ptr;
}

tetris::iterator tetris::begin(){
    return iterator(m_field);
}

tetris::iterator tetris::end(){
    return iterator(nullptr);
}

tetris::const_iterator tetris::begin() const{
    return const_iterator(m_field);
}

tetris::const_iterator tetris::end() const{
    return const_iterator(nullptr);
}

std::ostream& operator<<(std::ostream& os, tetris const& t){
    os << t.score() << " " << t.width() << " " << t.height() << "\n";

    for(auto const& pezzo : t){
        os << pezzo.p << " " << pezzo.x << " " << pezzo.y << "\n";
    }
    return os;
}

std::istream& operator>>(std::istream& is, tetris& t){
    uint32_t score = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    
    skip_whitespace(is);
    if(!(is >> score >> width >> height))
        throw tetris_exception("Impossibile leggere header");
    
    struct node_temp{
        piece p;
        int x;
        int y;
        node_temp* next;
    };
    
    node_temp* head = nullptr;
    
    skip_whitespace(is);
    while(is.peek() != std::char_traits<char>::eof()){
        piece p;
        int x = 0;
        int y = 0;
        
        is >> p;
        
        skip_whitespace(is);
        if(!(is >> x >> y)){
            while(head){
            	node_temp* old = head;
             	head = head->next; 
             	delete old;
            }
            throw tetris_exception("Errore lettura coordinate");
        }
        
        head = new node_temp{std::move(p), x, y, head};
        skip_whitespace(is);
    }
    
    tetris temp(width, height, score);
    while(head){
        temp.add(head->p, head->x, head->y);
        node_temp* old = head;
        head = head->next;
        delete old;
    }
    
    t = std::move(temp);
    return is;
}